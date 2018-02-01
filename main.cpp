#include <cstdio>
#include <cstring>
#include <map>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
using namespace std;

class Data
{
public:
	double inp[300][960];
	double out[300][20];
	double vinp[300][960];
	double vout[300][20];
	char train[100], valid1[100], valid2[100];
	int prob;

	int dataSize,dataSize1,dp,dp1;
	map<string,int> m;

	void Init(int p)
	{
		int i;
		prob =p;
		if(p==3)
		{
			sprintf(train,"lists/all_train.list");
			sprintf(valid1,"lists/all_test1.list");
			sprintf(valid2,"lists/all_test2.list");
		}
		else if(p==2)
		{
			sprintf(train,"lists/straighteven_train.list");
			sprintf(valid1,"lists/straighteven_test1.list");
			sprintf(valid2,"lists/straighteven_test2.list");
		}
		else if(p==1)
		{
			sprintf(train,"lists/straightrnd_train.list");
			sprintf(valid1,"lists/straightrnd_test1.list");
			sprintf(valid2,"lists/straightrnd_test2.list");
		}
		string s[20] ={"an2i","at33","boland","bpm","ch4f","cheyer","choon","danieln","glickman","karyadi","kawamura","kk49","megak","mitchell","night","phoebe""saavik","steffi","sz24","tammo"};
		for(i=0;i<20;i++)
			m[s[i]]=i;
	}

	void readImage(char* src, unsigned char* buf, double *d)
	{
		int i;
		char p;
		FILE *f;
		f = fopen(src,"r");
		// printf("%s",src);
		// fflush(stdout);
		fread(buf,1,13,f);
		fread(buf,1,960,f);
		fclose(f);

		for(i=0;i<960;i++)
			d[i]=((double)buf[i])/156;

		// for(i=0;i<960;i++)
		// 	printf("%lf ",d[i]);
		// printf("\n\n");
	}

	void readOutput(char *src, double *d)
	{
		int j;
		if(prob==3)
		{
			for(j=0;j<4;j++)
				d[j]=0.1;
			switch(*(strchr(src,'_')+1))
			{
				case 's':
					d[0] = 0.9;
					break;
				case 'u':
					d[1] = 0.9;
					break;
				case 'l':
					d[2] = 0.9;
					break;
				case 'r':
					d[3] = 0.9;
			}
		}
		else if(prob==2)
		{
			for(j=0;j<20;j++)
				d[j]=0.1;
			char *x = strchr(src,'/')+1;
			char *y=strrchr(src,'/');
			char s[20];
			j=0;
			while(x<y)
			{
			    s[j]=*x;
			    j++;
			    x++;
			}
			s[j]='\0';
			d[m[s]]=0.9;

		}
		else if(prob = 1)
		{
			d[0]=d[1]=0.1;
			char *x = strrchr(src,'_')-1;
			if((*x)=='n')
				d[0]=0.9;
			else
				d[1]=0.9;
		}

	}

	int loadTrainingData()
	{
		int i,j;
		unsigned char buf[1000];
		FILE *f;
		char src[200];

		f = fopen(train,"r");
		// printf("%s",train);
		// fflush(stdout);
		for(i=0;true;i++)
		{
			fgets(src,200,f);
			if(feof(f))
				break;
			src[strlen(src)-1]=0;
			readImage(src,buf,inp[i]);
			readOutput(src,out[i]);
		}
		fclose(f);
		dp = 0;
		dataSize = i;
		// for(i=0;i<1;i++)
		// {
		// 	for(j=0;j<960;j++)
		// 		printf("%lf\n",inp[i][j]);
		// 	printf("\n\n\n");
		// 	for(j=0;j<4;j++)
		// 		printf("%lf ",out[i][j]);
		// 	printf("\n---------------\n\n\n");
		// }
		return i;

	}

	int loadValidationData(int st)
	{
		dp1 = 0;
		int i,j;
		unsigned char buf[1000];
		char src[200];
		FILE *f;

		if(st==0)
			f = fopen(valid1,"r");
		else
			f = fopen(valid2,"r");

		for(i=0;true;i++)
		{
			fgets(src,200,f);
			if(feof(f))
				break;
			src[strlen(src)-1]=0;

			readImage(src,buf,vinp[i]);
			readOutput(src,vout[i]);
		}
		fclose(f);

		dataSize1 = i;
		return i;
	}

	pair<double *,double *> getTrainingData()
	{
		pair<double *,double*> p = make_pair(inp[dp],out[dp]);
		dp = (dp+1)%dataSize;
		return p;
	}

	pair<double *,double *> getValidationData()
	{
		pair<double *,double*> p = make_pair(vinp[dp1],vout[dp1]);
		dp1 = (dp1+1)%dataSize1;
		return p;
	}
};
class Face
{
public:
    Data D;
    double first_delta[960][20];
    double second_delta[20][20];
    double first_tho[20];
    double first[960][20];
    double second[20][20];
    double error,err;
    double mom;
    double eta;
    double gamma;
    double out1[960],out2[20],O[20];
    double *T;
    double *mat;
    double delta;
    int lim, lim2;
    void sigmoid_1()
    {
        double sum;
        int i,k=0;
        while(k<960)
        {
            out1[k]=mat[k];
            k++;
        }

    }
    void sigmoid_2()
    {
        double sum=0.00;
        int i,k=0;
        while(k<20)
        {
            sum=0.00;
            for(i=0;i<960;i++)
            {
                sum+=first[i][k]*out1[i];

            }
            out2[k]=1/(1+exp(-sum));
            k++;
        }
    }
    void sigmoid_3()
    {
        double sum=0.00;
        int i,k=0;
        while(k<20)
        {
            sum=0.00;
            for(i=0;i<20;i++)
            {
                sum+=second[i][k]*out2[i];
            }
            O[k]=1/(1+exp(-sum));
            k++;
        }

    }
    void delta_2()
    {
        int i=0,k=0;
        while(i<20)
        {
            k=0;
            while(k<20)
            {
                second_delta[i][k]=eta*(-((T[k]-O[k])*O[k]*(1-O[k])*out2[i])+2*gamma*second[i][k]) + mom*second_delta[i][k];
                k++;
            }
            i++;
        }
    }
    void delta_1()
    {
        double sum=0.00;
        int j,i=0,k=0;
        while(k<20)
        {
           sum=0.00;
           for(j=0;j<20;j++)
            {
                sum-=O[j]*(1-O[j])*(T[j]-O[j])*second[k][j];
            }
            first_tho[k]=out2[k]*(1-out2[k])*sum;
            k++;
        }
        while(i<960)
        {
            k=0;
            while(k<20)
            {
                first_delta[i][k]=eta*(first_tho[k]*out1[i]+2*gamma*first[i][k])+mom*first_delta[i][k];
                k++;
            }
            i++;
        }

    }
    void update_2()
    {
        int i,j;
        for(i=0;i<20;i++)
        {
            for(j=0;j<20;j++)
            second[i][j]=second[i][j]+second_delta[i][j];
        }
    }
    void update_1()
    {
        int i,j;
         for(i=0;i<960;i++)
        {
            for(j=0;j<20;j++)
            first[i][j]=first[i][j]+first_delta[i][j];
        }
    }
    Face()
    {
        int i,j;
        D.Init(2);
        mom=0.3;
        eta=-0.3;
        gamma=1/(exp(18));
        error=1;
        for(i=0;i<960;i++)
        {
            for(j=0;j<20;j++)
            {
                first[i][j]=(0.5+(((rand())%1000)*1.0)/1000) -1;
                first_delta[i][j]=0;
            }
        }
        for(i=0;i<20;i++)
        {
            for(j=0;j<20;j++)
            {
                second[i][j]=(0.5+(((rand())%1000)*1.0)/1000) -1;
                second_delta[i][j]=0;
            }
            first_tho[i]=0;
        }
        lim = D.loadTrainingData();
        lim2 = D.loadValidationData(0);

    }

    void train()
    {
        int k=0;
        int j=0;
        while(j<50)
        {
            k=0;
            while(k<lim)
            {
                pair<double*,double*> p = D.getTrainingData();
                mat=p.first;
                T=p.second;
                sigmoid_1();
                sigmoid_2();
                sigmoid_3();
                delta_2();
                delta_1();
                update_2();
                update_1();
                k++;
            }
            j++;
        }
    }
    void validateFinal()
    {
        lim2 = D.loadValidationData(1);
        validate();
    }
    void validate()
    {
        int k=0;
        error=0;
        while(k<lim2)
        {
            int i=0;
            int max_pos;
            pair<double*,double*> p = D.getValidationData();
            mat=p.first;
            T=p.second;
            sigmoid_1();
            sigmoid_2();
            sigmoid_3();
            double max=O[0];
            for(i=0;i<20;i++)
            {
                if(O[i]>=max)
                {
                    max=O[i];
                    max_pos=i;
                }
            }
            if(T[max_pos]<0.6)
            {
                error++;
            }
            k++;
        }
        error=error/lim2;
    }
};

double first[960][20],second[20][20];

// void print(char *file, int l,int m, int n, bool sec,double error)
// {
//     FILE *f = fopen(file,"w");
//     int i,j;
//     fprintf(f, "Accuracy %lf%%\n",(1-error)*100);

//     fprintf(f, "Weights in first level\n");
//     for(i=0;i<l;i++)
//     {
//         for(j=0;j<m;j++)
//         {
//             fprintf(f, "%lf,",first[i][j] );
//         }
//         fprintf(f, "\n");
//     }
//     fprintf(f, "\n");

//     if(sec)
//     {
//         fprintf(f,"Weights in second level\n");
//         for(i=0;i<m;i++)
//         {
//             for(j=0;j<n;j++)
//             {
//                 fprintf(f,"%lf,",first[i][j] );
//             }
//             fprintf(f, "\n");
//         }
//     }
// }

int main()
{
    Face p;
    int i,j;
    double perror,error;

    perror = 9999;
    error = 0;

    p.validate();
    error = p.error;
    printf("Initial accuracy %lf%%\n",(1-error)*100);

    for(int z=1;true;z++)
    {
        p.train();
        p.validate();
        error = p.error;
        printf("Accuracy after %d iterations %lf%%\n",z*50*277,(1-error)*100);
        if(perror <= error)
        {
            for(i=0;i<960;i++)
                for(j=0;j<20;j++)
                    p.first[i][j]=first[i][j];
            for(i=0;i<20;i++)
                for(j=0;j<20;j++)
                    p.second[i][j]=second[i][j];
            p.validateFinal();
            error = p.error;
            printf("Accuracy on final validation set %lf%%",(1-error)*100);
            // char fl[] = "face.csv";
            // print(fl,960,20,20,true,error);
            break;
        }
        for(i=0;i<960;i++)
            for(j=0;j<20;j++)
                first[i][j]=p.first[i][j];
        for(i=0;i<20;i++)
        {
            for(j=0;j<20;j++)
            {
                second[i][j]=p.second[i][j];
            }
        }
        perror = error;
    }
}
