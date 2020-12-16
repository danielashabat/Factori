// Includes --------------------------------------------------------------------

#include "ThreadsFunctions.h"
#include "Lock.h"
#include  "Queue.h"

// Implementation -------------------------------------------------------
BOOL Create_Thread_data(char tasks_file_path[], QUEUE* task_queue, Lock* lock, ThreadData** ptr_to_thread_data) {
	(*ptr_to_thread_data) = (ThreadData*)malloc(sizeof(ThreadData));
	if (*ptr_to_thread_data == NULL) {
		printf("ERROR: allocation failed!\n");
		return FAIL;
	}
	strcpy_s((*ptr_to_thread_data)->input_path, MAX_PATH, tasks_file_path);
	(*ptr_to_thread_data)->task_queue = task_queue;
	(*ptr_to_thread_data)->lock = lock;

	return TRUE;
}
int ThreadFunction(LPVOID lpParam) {
	int task_offset;
	int num;
	int counter_num_of_factors = 0;
	int* prime_factor_array = NULL;
	char* string_to_file = NULL;
	char tasks_file_name[MAX_PATH];

	QUEUE* tasks_queue = NULL;
	Lock* lock = NULL;

	HANDLE hfile_tasks = NULL;
	BOOL pass_or_fail = FALSE;

	ThreadData* p_params;
	p_params = (ThreadData*)lpParam;
	strcpy_s(tasks_file_name, MAX_PATH, p_params->input_path);
	tasks_queue = p_params->task_queue;
	lock = p_params->lock;

	//create new handle to tasks file in read and write mode 
	hfile_tasks= CreateFileA((LPCSTR)tasks_file_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	IF_FAILED_END_PROGRAM(check_file_handle(hfile_tasks, tasks_file_name))//check for failure


	while (!Empty(tasks_queue)) {
		task_offset = Pop(tasks_queue);
		pass_or_fail=read_num_from_tasks_file(hfile_tasks, &num, task_offset);
		IF_FAILED_END_PROGRAM(pass_or_fail)
		pass_or_fail = find_prime_factors(num, &prime_factor_array, &counter_num_of_factors);
		IF_FAILED_END_PROGRAM(pass_or_fail)
		create_string_to_write(&string_to_file, num, counter_num_of_factors, &prime_factor_array);

		pass_or_fail=write_to_tasks_file(hfile_tasks, num);
		IF_FAILED_END_PROGRAM(pass_or_fail)
		break;
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

BOOL write_to_tasks_file(HANDLE hfile_tasks, int num) {
	BOOL bErrorFlag = FALSE;
	BOOL pass_or_fail = FALSE;
	
	DWORD lpNumberOfBytesWritten=0;
	DWORD nNumberOfBytesToWrite = 0;

	
	SetFilePointer(hfile_tasks,0, NULL, FILE_END);//set pointer to end of file
	if (NULL != string_to_file) nNumberOfBytesToWrite = strlen(string_to_file);//////need to check length with ANAT
	printf("size: %d\n", nNumberOfBytesToWrite);

	bErrorFlag = WriteFile(hfile_tasks, string_to_file, nNumberOfBytesToWrite, &lpNumberOfBytesWritten, NULL);
	IF_FAILED_END_PROGRAM(check_ReadFile_WriteFile(bErrorFlag, nNumberOfBytesToWrite, lpNumberOfBytesWritten))

	free(prime_factor_array);
	free(string_to_file);
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