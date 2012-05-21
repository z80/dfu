March 13, 2010  FatFs R0.08.dev.1


1. LFN Feature with dynamic working buffer on the HEAP

#define	 _USE_LFN	3	// Setting 3 enables this feature

Also user defined OS dependent memory functions, ff_memalloc and ff_memfree, are required. There is a sample code in the syscall.c.




2. File Shareing Feature (File Lock)

#define _FS_SHARE <n>

Setting _FS_SHARE to 1 or larger enables the file shareing feature and the value determins number of open files per volume. It can work regardless setting of reentrant feature. The following sequense is an example with _FS_SHARE == 3.

	f_open(&f1 , "foo.txt", FA_READ);	// FR_OK
	f_open(&f2 , "foo.txt", FA_READ);	// FR_OK
	f_open(&f3 , "foo.txt", FA_WRITE);	// FR_LOCKED
	f_close(&f1);				// FR_OK
	f_close(&f2);				// FR_OK
	f_open(&f3 , "foo.txt", FA_WRITE);	// FR_OK
	f_open(&f1 , "foo.txt", FA_READ);	// FR_LOCKED
	f_unlink("foo.txt");			// FR_LOCKED
	f_rename("foo.txt", "new.txt");		// FR_LOCKED
	f_close(&f3);				// FR_OK
	f_open(&f1 , "foo1.txt", FA_READ);	// FR_OK
	f_open(&f2 , "foo2.txt", FA_READ);	// FR_OK
	f_open(&f3 , "foo3.txt", FA_READ);	// FR_OK
	f_open(&f4 , "foo4.txt", FA_READ);	// FR_TOO_MANY_OPEN_FILES




3. Fast Seek with On-memory Cluster Link Map

FRESULT f_lseek (
  FIL* FileObject,   /* Pointer to the file object structure */
  DWORD Offset,      /* File offset in unit of byte */
  DWORD *Map         /* Pointer to the cluster link map (NULL:normal seek) */
);

The f_lseek function has been changed to implement fast seek feature. But the additional 3rd argument is ignored unless fast seek feature is enabled (_USE_FASTSEEK == 1). To use this feature, give a pointer to the working buffer to store the cluster link map to the 3rd argument.


	DWORD seektbl[10];	// Cluster link map table. A file fragmented in N requires (N + 1) * 2 items).


	res = f_oepn(&file, "foo.avi", FA_READ);		// Open a file
	if (res != FR_OK) die(res);

	res = f_lseek(&file, somewhere, 0);			// This is a normal seek

	seektbl[0] = 10;					// Set table size (1st item)
	res = f_lseek(&file, CREATE_LINKMAP, seektbl);		// Create link map table
	if (res != FR_OK) die(res);

	for (i = 1, cc = 0; seektbl[i]; cc += seektbl[i], i += 2) ;
	printf("%d clusters, fragmented in %d.\n", cc, i / 2);	// Show status

	....

	res = f_lseek(&file, somewhere, seektbl);		// Seek without FAT access

