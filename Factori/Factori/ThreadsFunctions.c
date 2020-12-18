// Includes --------------------------------------------------------------------

#include "ThreadsFunctions.h"
#include "Lock.h"
#include  "Queue.h"

// Defines -------------------------------------------------------------------
#define TIMEOUT = ((int)5000)

// Implementation -------------------------------------------------------
BOOL Create_Thread_data(char tasks_file_path[], QUEUE* task_queue, Lock* lock,HANDLE queue_mutex, ThreadData** ptr_to_thread_data) {
	(*ptr_to_thread_data) = (ThreadData*)malloc(sizeof(ThreadData));
	if (*ptr_to_thread_data == NULL) {
		printf("ERROR: allocation failed!\n");
		return FAIL;
	}
	strcpy_s((*ptr_to_thread_data)->input_path, MAX_PATH, tasks_file_path);
	(*ptr_to_thread_data)->task_queue = task_queue;
	(*ptr_to_thread_data)->lock = lock;
	(*ptr_to_thread_data)->queue_mutex = queue_mutex;
	return TRUE;
}
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
	int task_offset;
	int num;
	int counter_num_of_factors = 0;
	
	char tasks_file_name[MAX_PATH];
	DWORD wait_code;
	BOOL ret_val;
	HANDLE queue_mutex_handle = NULL;
	QUEUE* tasks_queue = NULL;
	Lock* lock = NULL;
	BOOL queue_is_empty = FALSE; 
	BOOL start_read_and_write = FALSE;
	HANDLE hfile_tasks = NULL;
	BOOL pass_or_fail = FALSE;

	ThreadData* p_params;
	p_params = (ThreadData*)lpParam;
	strcpy_s(tasks_file_name, MAX_PATH, p_params->input_path);
	tasks_queue = p_params->task_queue;
	lock = p_params->lock;
	queue_mutex_handle = p_params->queue_mutex;

	//create new handle to tasks file in read and write mode 
	hfile_tasks= CreateFileA((LPCSTR)tasks_file_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	IF_FAILED_END_PROGRAM(check_file_handle(hfile_tasks, tasks_file_name));//check for failure

	while (!queue_is_empty) {
		pass_or_fail = check_queue_status(queue_mutex_handle, tasks_queue, &task_offset, &queue_is_empty);
		if (!queue_is_empty) {
			//Read taske from file
			int* prime_factor_array = NULL;
			char* string_to_file = NULL;
			pass_or_fail = read_lock(lock, 10000);
			printf("can read file \n");
			IF_FAILED_END_PROGRAM(pass_or_fail);
			pass_or_fail = read_num_from_tasks_file(hfile_tasks, &num, task_offset);
			printf("end read critical section \n");
			printf("%d num from file\n", num);
			IF_FAILED_END_PROGRAM(pass_or_fail);
			pass_or_fail = read_release(lock, 10000);
			IF_FAILED_END_PROGRAM(pass_or_fail);

			//Analyze the string - not need to be in critical section 
			IF_FAILED_END_PROGRAM(pass_or_fail)
			pass_or_fail = find_prime_factors(num, &prime_factor_array, &counter_num_of_factors);
			IF_FAILED_END_PROGRAM(pass_or_fail)
			pass_or_fail = create_string_to_write(&string_to_file, num, counter_num_of_factors, &prime_factor_array);
			IF_FAILED_END_PROGRAM(pass_or_fail);
			printf("%s", string_to_file);


			//write task to file
			pass_or_fail = write_lock(lock, counter_num_of_factors*10000);
			IF_FAILED_END_PROGRAM(pass_or_fail);
			pass_or_fail = write_to_tasks_file(hfile_tasks, string_to_file, num);
			printf("end file critical section \n");
			IF_FAILED_END_PROGRAM(pass_or_fail);
			pass_or_fail = write_release(lock, counter_num_of_factors*10000);

			IF_FAILED_END_PROGRAM(pass_or_fail);

			free(prime_factor_array);
			free(string_to_file);


			
		}
	}
	CloseHandle(hfile_tasks);
	
	
	return 0;
}


BOOL read_num_from_tasks_file(HANDLE hfile_tasks, int *num, LONG offset) {

	*num = 0;
	int mult = 10;
	int digit = 0;
	int dw_pointer;
	char char_buffer;
	DWORD nBytesToRead = 1;
	DWORD dwBytesRead = 0;
	BOOL bResult = FALSE;

	dw_pointer = SetFilePointer(hfile_tasks, offset, NULL, FILE_BEGIN);//set poniter to offset

	while (!bResult || (dwBytesRead != 0)) {//loop until EOF
		bResult = ReadFile(hfile_tasks, &char_buffer, nBytesToRead, &dwBytesRead, NULL);
		IF_FAILED_END_PROGRAM(bResult)
		if (char_buffer == '\n' || char_buffer == '\r' ) {
			break;//reach to end of line
		}
		digit = char_buffer - 48;//tranform char to digit 
		*num= digit + mult*(*num);
	}
	printf("READ from tasks file success! the num for task is %d\n", *num);
	return SUCCSESS;
}

BOOL write_to_tasks_file(HANDLE hfile_tasks,char* string_to_file, int num) {
	BOOL bErrorFlag = FALSE;
	BOOL pass_or_fail = FALSE;
	
	DWORD lpNumberOfBytesWritten=0;
	DWORD nNumberOfBytesToWrite = 0;

	
	SetFilePointer(hfile_tasks,0, NULL, FILE_END);//set pointer to end of file
	if (NULL != string_to_file) nNumberOfBytesToWrite = strlen(string_to_file);//////need to check length with ANAT
	printf("size: %d\n", nNumberOfBytesToWrite);

	bErrorFlag = WriteFile(hfile_tasks, string_to_file, nNumberOfBytesToWrite, &lpNumberOfBytesWritten, NULL);
	IF_FAILED_END_PROGRAM(check_ReadFile_WriteFile(bErrorFlag, nNumberOfBytesToWrite, lpNumberOfBytesWritten))

	
	return SUCCSESS;
}

BOOL check_file_handle(HANDLE h_file, char* file_name) {
	if (h_file == INVALID_HANDLE_VALUE) {

		printf("Could not open %s file, error %ld\n", file_name, GetLastError());
		return FAIL;
	}
	else {

		printf("%s file HANDLE is OK!\n", file_name);
		return SUCCSESS;

	}
}

BOOL check_ReadFile_WriteFile(BOOL bErrorFlag, DWORD number_of_bytes_to_read_or_write, DWORD lpNumberOfBytesRead_or_Written) {

	if (FALSE == bErrorFlag)
	{
		printf("Terminal failure: Unable to write to file.\n");
		return FAIL;
	}
	else
	{
		if (number_of_bytes_to_read_or_write != lpNumberOfBytesRead_or_Written)
		{
			// This is an error because a synchronous write that results in
			// success (WriteFile returns TRUE) should write all data as
			// requested. This would not necessarily be the case for
			// asynchronous writes.
			printf("Error: dwBytesWritten != dwBytesToWrite\n");
		}
		else
		{
			printf("Read or Write successfully.\n");
		}
		return SUCCSESS;
	}



}
BOOL  find_prime_factors(int num, int** prime_factor_array, int* counter_num_of_factors) {

	int i = 3;
	int* p_prime_factors = NULL;
	int counter = 0;

	p_prime_factors = (int*)malloc(sizeof(int));
	if (p_prime_factors == NULL) {
		return FALSE;
	}
	while (num % 2 == 0) {
		if (p_prime_factors == NULL) {
			return FALSE;
		}
		p_prime_factors[counter] = 2;
		num = (num / 2);
		counter++;
		p_prime_factors = (int*)realloc(p_prime_factors, (counter + 1) * sizeof(int));
		if (p_prime_factors == NULL) {
			return FALSE;
		}
	}

	while (i <= sqrt(num)) {
		while (num % i == 0) {///divide in zero ERROR
			p_prime_factors[counter] = i;
			num = (num / i);
			counter++;
			p_prime_factors = (int*)realloc(p_prime_factors, (counter + 1) * sizeof(int));
			if (p_prime_factors == NULL) {
				return FALSE;
			}
		}
		i = i + 2;
	}
	if (num > 2) {

		(p_prime_factors)[counter] = num;
		counter++;
	}
	*counter_num_of_factors = counter;
	*prime_factor_array = p_prime_factors;
	return TRUE;
}

BOOL create_string_to_write(char** string, int num, int num_of_factors, int** factor_array) {



	int len_of_string = 27 + 9 + (5 * num_of_factors) + num_of_factors * 2;
	*string = (char*)malloc(sizeof(char) * len_of_string);
	sprintf_s(*string, len_of_string, "The prime factors of %d are:", num);
	if (*string == NULL) {
		return FALSE;
	}



	char buffer_str_factors[8];
	for (int i = 0; i < num_of_factors; i++) {

		if (i != num_of_factors - 1) {

			sprintf_s(buffer_str_factors,8, " %d,", (*factor_array)[i]);
			strcat_s(*string, len_of_string, buffer_str_factors);
		}
		else {
			sprintf_s(buffer_str_factors, 8," %d\r\n", (*factor_array)[i]);
			strcat_s(*string, len_of_string, buffer_str_factors);
		}

	}
	return TRUE;
}

int get_number_of_digits(num) {
	int count = 0;
	while (num != 0) {
		count++;
		num = num /= 10;
	}
	return count;
}


BOOL check_queue_status(HANDLE queue_mutex_handle,QUEUE* tasks_queue, int* task_offset,BOOL* queue_is_empty) {
	DWORD wait_code;
	BOOL ret_val;
	BOOL pass_or_fail;
	/* Create the mutex that will be used to synchronize access to queue */
	wait_code = WaitForSingleObject(queue_mutex_handle, INFINITE);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("-ERROR: %d - WaitForSingleObject failed !\n", GetLastError());
		return FALSE;
	}
	//critical section- check if queue is not empty and pop the mission 
	if (!Empty(tasks_queue)) {
		*task_offset = Pop(tasks_queue);
		printf("%d\n", *task_offset);
		*queue_is_empty = FALSE;
	}
	else {
		*queue_is_empty = TRUE;
		*task_offset = -1; 
	}
	//end of critical section 
	//*Release queue mutex
	printf("end queue critical section %d\n", task_offset);
	ret_val = ReleaseMutex(queue_mutex_handle);
	if (FALSE == ret_val)
	{
		printf("-ERROR: %d - release semaphore failed !\n", GetLastError());
		return FALSE;
	}

}

BOOL read_and_write() {



}