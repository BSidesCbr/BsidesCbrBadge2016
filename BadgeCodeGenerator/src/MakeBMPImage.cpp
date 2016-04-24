#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int
main(int argc, char *argv[])
{
	int fd;
	unsigned int sz, count;
	unsigned char *buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Can't open %s\n", argv[1]);
		exit(1);
	}
	sz = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	buf = (unsigned char *)malloc(sz);
	if (buf == NULL) {
		fprintf(stderr, "Can't allocate memory...\n");
		exit(1);
	}
	if (read(fd, buf, sz) != sz) {
		fprintf(stderr, "Read error\n");
		exit(1);
	}
	printf("#ifndef MYFILE_H\n");
	printf("#define MYFILE_H\n");
	printf("\n");
	printf("unsigned char myfile[] = {\n");
	count = 0;
	for (unsigned int i = 0; i < (sz - 1);) {
		if (buf[i] == 0) {
			unsigned int j;
			int notended = 1;

			do {
				j = i + 1;
				while (buf[j] == 0 && (j - i) < 255 && j < (sz - 1)) j++;
				printf("0x0, 0x%x, ", j - i);
				count += 2;
				if ((j - i) != 255)
					notended = 0;
				i = j;
			} while (notended);
			
		} else {
			printf("0x%x, ", buf[i++]);
			count++;
		}
	}
	printf("0x%x\n", buf[sz - 1]);
	count++;
	printf("};\n");
	printf("#define MYFILESZ %u\n", count);
	printf("\n");
	printf("#endif\n");
	close(fd);
}
