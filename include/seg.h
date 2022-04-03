#ifndef SEG_H
#define SEG_H

typedef enum {
	SEG_CS, // code 
	SEG_DS, // data 
	SEG_SS, // stack
	SEG_FS, // g/p segment
	SEG_GS,	// g/p segment
	SEG_UNKNOWN
} segmenttype_t;

typedef struct {
	segmenttype_t type;
	unsigned short vbase; // virtual base (max seg size is ushort_max)
} segment_t;

void segment_initstate();
segment_t* segment_create(segmenttype_t type, unsigned long vbase);
segmenttype_t segment_resove_type(unsigned long vbase);
unsigned short segment_resolve_base(segmenttype_t type);

#endif
