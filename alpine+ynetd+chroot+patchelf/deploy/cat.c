#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int cat_file(const char *file) {
	char buf[1024] = {0};
	int fd = open(file, O_RDONLY);
	if (fd == -1) {
		printf("[!] Fail to open file [%s]\n", file);
		return -1;
	}
	int n = -1;
	while ((n = read(fd, buf, 1024)) > 0) {
		write(STDOUT_FILENO, buf, n);
	}

	return 0;
}

int cat_files(const char *files[], int n) {
	for (int i=0; i<n; i++) {
		int retval = cat_file(files[i]);
		if (retval == -1) {
			return -1;
		}
	}

	return 0;
}

int main(int argc, const char *argv[]) {
	return cat_files(argv+1, argc-1);
}