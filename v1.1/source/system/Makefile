
MAKE		= make -r
DEL			= @rm -f

clean :
	-$(DEL) ./*.bin
	-$(DEL) ./*.lst
	-$(DEL) ./*.o
	-$(DEL) ./*.map
	-$(DEL) ./*.bim
	-$(DEL) ./*.hrb
	-$(DEL) ./*.sys
	-$(DEL) ./*.ii
	-$(DEL) ./*.s
	$(MAKE) -C ./gcc clean
	$(MAKE) -C ./fifo clean
	$(MAKE) -C ./graphic clean
	$(MAKE) -C ./inputdev clean
	$(MAKE) -C ./memory clean
	$(MAKE) -C ./obj clean
	$(MAKE) -C ./ssememory clean
	$(MAKE) -C ./task clean
	$(MAKE) -C ./taskctl clean
	$(MAKE) -C ./task/main_task clean
	$(MAKE) -C ./timer clean
