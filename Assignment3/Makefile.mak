obj-m += char_dev_in.o
obj-m += char_dev_out.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules