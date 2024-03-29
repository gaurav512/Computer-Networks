/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "calc.h"
#include <rpc/rpc.h>
#include <stdio.h>

void
calc_3(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	values  add_3_arg;
	int  *result_2;
	values  sub_3_arg;
	char opt;


	printf("Enter operation to perform (+ or -): ");	
	scanf("%c", &opt);
	
	if(opt == '+')
	{
		printf("Enter two numbers:\n");
		scanf("%d", &add_3_arg.num1);
		scanf("%d", &add_3_arg.num2);
	}
	else if(opt == '-')
	{
		printf("Enter two numbers:\n");
		scanf("%d", &sub_3_arg.num1);
		scanf("%d", &sub_3_arg.num2);
	}
#ifndef	DEBUG
	clnt = clnt_create (host, CALC, CALC_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	switch(opt)
	{

		case '+':
		result_1 = add_3(&add_3_arg, clnt);
		if (result_1 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
		printf("Answer is: %d\n", *result_1);
		break;
		
		case '-':
		result_2 = sub_3(&sub_3_arg, clnt);
		if (result_2 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
		printf("Answer is: %d\n", *result_2);
		break;

		default:
		printf("Invalid input!\n");	
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	calc_3 (host);
exit (0);
}
