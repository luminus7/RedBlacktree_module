#----------Makefile-----------#

obj-m	:= rb_module_sj.o

# googling: kernel module compile with multiple files. (stack_ovflw)
rb_module_sj-objs := ./sj_queue.o ./sj_rbtest.0

KERNEL_DIR	:= /lib/modules/$(shell uname -r)/build
PWD	:=$(shell pwd)

default:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
clean :
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean