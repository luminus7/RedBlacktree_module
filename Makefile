#----------Makefile-----------#

obj-m	:= rb_mult_module.o

# googling: kernel module compile with multiple files. (stack_ovflw)
rb_mult_module-objs := ./sj_queue.o ./sj_rbtest.o

KERNEL_DIR	:= /lib/modules/$(shell uname -r)/build
PWD	:=$(shell pwd)

default:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
clean :
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean