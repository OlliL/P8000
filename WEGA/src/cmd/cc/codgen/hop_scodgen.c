/* scodgen: hoptab, floating and integer */
struct hoptab fhoptab[] = {
	{ ASG PLUS, "fadd", TFLOAT|TDOUBLE },
	{ ASG MINUS, "fsub", TFLOAT|TDOUBLE },
	{ ASG MUL, "fmul", TFLOAT|TDOUBLE },
	{ ASG DIV, "fdiv", TFLOAT|TDOUBLE },
	{ PLUS, "fadd", TFLOAT|TDOUBLE },
	{ MINUS, "fsub", TFLOAT|TDOUBLE },
	{ MUL, "fmul", TFLOAT|TDOUBLE },
	{ DIV, "fdiv", TFLOAT|TDOUBLE },
	{ -1, 0, 0 }
};

struct hoptab ihoptab[] = {
	{ ASG PLUS, "add", TANY },
	{ ASG MINUS, "sub", TANY },
	{ ASG OR, "or", TANY },
	{ ASG AND, "and", TANY },
	{ ASG ER, "xor", TANY },
	{ ASG MUL, "mul", TANY },
	{ ASG DIV, "div", TANY },
	{ ASG MOD, "div", TANY },
	{ ASG LS, "sla", TANY },
	{ ASG RS, "srl", TUCHAR|TUSHORT|TUNSIGNED|TULONG },
	{ ASG RS, "sra", TANY },
	{ PLUS, "add", TANY },
	{ MINUS, "sub", TANY },
	{ OR, "or", TANY },
	{ AND, "and", TANY },
	{ ER, "xor", TANY },
	{ MUL, "mul", TANY },
	{ DIV, "div", TANY },
	{ MOD, "div", TANY },
	{ RS, "srl", TUCHAR|TUSHORT|TUNSIGNED|TULONG },
	{ LS, "sla", TANY },
	{ RS, "sra", TANY },
	{ -1, 0, 0 }
};

