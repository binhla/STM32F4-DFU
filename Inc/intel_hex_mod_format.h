#ifndef __INTEL_HEX_MOD_FORMAT_H
#define __INTEL_HEX_MOD_FORMAT_H

/*
 :llaaaatt[dd...]cc
 :----------------------- the colon start every Intel HEX record
  ll--------------------- the number of data bytes in [dd...]
    aaaa----------------- the starting address for subsequent data in the record
		    tt--------------- HEX record type
				  [dd...]-------- data field
					       cc------ checksum
 */
 
 /*
  HEX record type
	00 data record
	01 end of file record
	02 extended segment address record
	04 extended linear address record
	05 start linear address record (MDK-ARM only)
	*/
	
	

#endif /*__INTEL_HEX_MOD_FORMAT_H*/
