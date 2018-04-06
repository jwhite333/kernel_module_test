obj-m := test_module.o

KERNEL_DIR = /lib/modules/$(shell uname -r)/build
CUR_DIR = $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CUR_DIR) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order .test* .tmp_*