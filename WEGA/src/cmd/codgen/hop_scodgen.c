/* scodgen: hoptab, floating and integer */
struct hoptab { int op; char *str; int tword; };

struct hoptab fhoptab[] = {
	{ 7, "fadd", 0x30 },
	{ 9, "fsub", 0x30 },
	{ 12, "fmul", 0x30 },
	{ 61, "fdiv", 0x30 },
	{ 6, "fadd", 0x30 },
	{ 8, "fsub", 0x30 },
	{ 11, "fmul", 0x30 },
	{ 60, "fdiv", 0x30 },
	{ -1, 0, 0 }
};

struct hoptab ihoptab[] = {
	{ 7, "add", 0x1000 },
	{ 9, "sub", 0x1000 },
	{ 18, "or", 0x1000 },
	{ 15, "and", 0x1000 },
	{ 20, "xor", 0x1000 },
	{ 12, "mul", 0x1000 },
	{ 61, "div", 0x1000 },
	{ 63, "div", 0x1000 },
	{ 65, "sla", 0x1000 },
	{ 67, "srl", 0x780 },
	{ 67, "sra", 0x1000 },
	{ 6, "add", 0x1000 },
	{ 8, "sub", 0x1000 },
	{ 17, "or", 0x1000 },
	{ 14, "and", 0x1000 },
	{ 19, "xor", 0x1000 },
	{ 11, "mul", 0x1000 },
	{ 60, "div", 0x1000 },
	{ 62, "div", 0x1000 },
	{ 66, "srl", 0x780 },
	{ 64, "sla", 0x1000 },
	{ 66, "sra", 0x1000 },
	{ -1, 0, 0 }
};

