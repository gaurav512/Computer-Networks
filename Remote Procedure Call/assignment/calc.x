struct values {
	int num1;
	int num2;
};

program CALC {
	version CALC_VERS {
		int ADD(values) = 1;
		int SUB(values) = 2;
	} = 3;
} = 0x31111111;
