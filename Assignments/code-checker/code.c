#include <stdio.h>
#include <stdlib.h>

int main()
{
    int t;
    scanf("%d", &t);
    while(t--)
    {
        int n;
        scanf("%d", &n);
        int val = 1;
        for(int i=0; i<n; i++)
        {
            for(int j=0; j<=i; j++)
            {
                printf("%d ", val++);
            }
            printf("\n");
        }
    }
    return 0;
}