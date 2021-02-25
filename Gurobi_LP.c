/*building a model for Gurobi optimization using the LP method*/

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "Board.h"
#include "GurobiAux.h"
extern Board *board;

/* function for the commands guess and guess_hint
 * parameters: solution array after gurobi LP optimization , row and column of the cell we want a hint for,
 * threshold for guess function, command we want to execute, the valid values board that we created for the LP optimization
 * if command = 'h' we execute the guess_hint command, if command ='g' we execute the guess command.
 * */
void guess_hint_GRB(double *sol, int x,int y,float threshold,char command,int *** VVBoard){
	int i,j,k,cnt,m,s,q,tmp=0,dim=board->size,r,c;
	float sum =0.0;
	int row=x;
	int col=y;
	int * values_array;
	float * prob_array;
	x+=1;
	y+=1;
	if(command=='h'){
		if(VVBoard[row][col]!=NULL){
			printf("the legal values and their scores for cell <%d,%d> are:\n",x,y);
			for(i=0;i<VVBoard[row][col][0];i++){
				if(sol[VVBoard[row][col][i*2+2]]>0.0){
					printf("value: %d score: %f\n",VVBoard[row][col][i*2+1],sol[VVBoard[row][col][i*2+2]]);
				}
			}
		}
		else{
			printf("there are no legal values for this cell\n");
		}
	}
	else if(command=='g'){
		for(i=0;i<dim;i++){
			for(j=0;j<dim;j++){
				cnt=0; /*for each cell we counting the number legal values with score greater than the threshold*/
				if(VVBoard[i][j]!=NULL){
					for(k=0;k<VVBoard[i][j][0];k++){
						if(sol[VVBoard[i][j][k*2+2]]>threshold){
							cnt++;
						}
					}
					if(cnt!=0){
						values_array=(int*) malloc(cnt*sizeof(int)); /* contain the legal values with score greater than the threshold*/
						if (values_array==NULL){
							printf("Error: failed to allocate memory.\n");
							exit(1);
						}
						prob_array=(float*) malloc(cnt*sizeof(float)); /* contain the legal values scores with score greater than the threshold*/
						if (prob_array==NULL){
							printf("Error: failed to allocate memory.\n");
							exit(1);
						}
						m=0;
						for(k=0;k<VVBoard[i][j][0];k++){
							if(sol[VVBoard[i][j][k*2+2]]>threshold){
								values_array[m]=VVBoard[i][j][2*k+1];
								prob_array[m]=sol[VVBoard[i][j][2*k+2]];
								sum+=sol[VVBoard[i][j][2*k+2]];
								m++;
							}
						}
						for(k=0;k<cnt;k++){
							prob_array[k]=prob_array[k]/sum; /*gives the right proportion for each value with probability better
							than the threshold (normalize the sum of probabilities to one)*/
							if(k!=0){
								prob_array[k]=prob_array[k]+prob_array[k-1];
							}
						}
						sum=(float)rand() / RAND_MAX;
						for(k=0;k<cnt;k++){
							if(prob_array[k]>=sum){
								board->cells[i][j].val=values_array[k];
								tmp=values_array[k];
								break;
							}
						}
						/*for every cell in the column that has valid value witch is the same value that we just put in the board we change his probability to 0;*/
						for(s=0;s<dim;s++){
							if(VVBoard[s][j]!=NULL){
								for(k=0;k<VVBoard[s][j][0];k++)	{
									if(VVBoard[s][j][2*k+1]==tmp){
										sol[VVBoard[s][j][2*k+2]]=0.0;
									}
								}
							}
														}
						/*for every cell in the row that has valid value witch is the same value that we just put in the board we change his probability to 0;*/
						for(s=0;s<dim;s++){
							if(VVBoard[i][s]!=NULL){
								for(k=0;k<VVBoard[i][s][0];k++)	{
									if(VVBoard[i][s][2*k+1]==tmp){
										sol[VVBoard[i][s][2*k+2]]=0.0;
									}
								}
							}
														}
						/*for every cell in the block that has valid value witch is the same value that we just put in the board we change his probability to 0;*/
						r=i-(i%board->rowsinblock);
						c=j-(j%board->colsinblock);
						for(s=r;s<r+board->rowsinblock;s++){
							for(q=c;q<c+board->colsinblock;q++){
								if(VVBoard[s][q]!=NULL){
									for(k=0;k<VVBoard[s][q][0];k++)	{
										if(VVBoard[s][q][2*k+1]==tmp){
											sol[VVBoard[s][q][2*k+2]]=0.0;
										}
									}
								}
							}
						}
						free(values_array);
						free(prob_array);

					}


				}
			}
		}
	}
}
/*building a model for the gurobi optimizer and running the optimizer
 * if the optimizer found a legal solution for the board according to the LP method we call the guess_hint_GRB function
 * the function returns 1 if the optimization succeed and 0 if it failed (s.t. the board is unsolvable)*/

int gurobiLP(int x,int y,float threshold,char command)
{
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  int       dim=board->size; /*the board dimension*/
  double *   sol;	    /* values of the variables if solution has been found*/
  int    *   ind;     /* index of constrain*/
  double *   val;	  /*coefficient of constrain*/
  int ***   VVBoard;
  double *  obj;	 /*objective function*/
  char   *  vtype;  /*variable types of objective function*/
  int       optimstatus;
  double    objval;
  int *randarray;
  int i,j,k,m,n,ib,jb,rowdim,coldim,randVar,position,count,totalNumOfVars;
  int ret=0;

  VVBoard=(int***)malloc(dim*sizeof(int**));
  if (VVBoard==NULL){
  		printf("Error: failed to allocate memory.\n");
  		exit(1);
  	}
  totalNumOfVars=validValuesBoard(VVBoard); /*count number of required variables + set valid values board*/
  rowdim=board->rowsinblock;
  coldim=board->colsinblock;

  obj=(double*)malloc(sizeof(double)*totalNumOfVars);
	if (obj==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
  vtype=(char*)malloc(sizeof(char)*totalNumOfVars);
	if (vtype==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	sol=(double*)malloc(sizeof(double)*totalNumOfVars);
		if (sol==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
	ind=(int*)malloc(dim*sizeof(int));
	if (ind==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	val=(double*)malloc(dim*sizeof(double));
	if (val==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}



  /* Create environment - log file is mip1.log */
  error = GRBloadenv(&env, "mip1.log");
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }
  
  error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }

  /* Create an empty model named "mip1" */
  error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }

  /* Add variables */
  
  /* coefficients - for all empty cells legal values in the board, the coefficients created with random weights*/
  for(i=0;i<dim;i++){
	  for(j=0;j<dim;j++){
		  if(VVBoard[i][j]!=NULL){
		  randVar=VVBoard[i][j][0];
		  randarray=(int*)malloc(sizeof(int)*randVar);
			if (randarray==NULL){
				printf("Error: failed to allocate memory.\n");
				exit(1);
			}
		  for(n=0;n<randVar;n++){
			  randarray[n]=n+1;
		  }
		  for(k=1;k<dim+1;k++){
			  for(m=0;m<VVBoard[i][j][0];m++){
				  if(k==VVBoard[i][j][2*m+1]){
					  position=rand()%randVar;
					  obj[VVBoard[i][j][2*m+2]]=randarray[position];
					  randarray[position]=randarray[randVar-1]; /*replaces variables in the array so we do not use the same coef twice.*/
					  randVar--;
					  vtype[VVBoard[i][j][2*m+2]]=GRB_CONTINUOUS;
					  break;
			  }

			  }

		  }
		  free(randarray);
		  }
	  }
  }
  
  
  /* add variables to model */
  error = GRBaddvars(model, totalNumOfVars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }


  /* Change objective sense to maximization */
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }

  /* update the model - to integrate new variables */

  error = GRBupdatemodel(model);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }
  /*each variable is between 0 and 1*/
  for(i=0;i<totalNumOfVars;i++){
	  ind[0]=i;
	  val[0]=1;
	  error = GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
	  				  if (error) {
	  					  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  					  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
	  					  GRBfreemodel(model);
	  					  GRBfreeenv(env);
	  					  freeValidValuesBoard(VVBoard);
	  					  free(obj);
	  					  free(vtype);
	  					  free(ind);
	  					  free(sol);
	  					  free(val);
	  					  return -1;
	  				  }
	error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
						  if (error) {
							  printf("Error: the command was not executed because of the following error in Gurobi:\n");
							  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
							  GRBfreemodel(model);
							  GRBfreeenv(env);
							  freeValidValuesBoard(VVBoard);
							  free(obj);
							  free(vtype);
							  free(ind);
							  free(sol);
							  free(val);
							  return -1;
						  }
  }


  /* Each empty cell with at least one valid value gets a value */
  for(i=0;i<dim;i++){
 	  for(j=0;j<dim;j++){
 		  count=0;
 		  if(VVBoard[i][j]!=NULL){
			for(k=1;k<dim+1;k++){
				for(m=0;m<VVBoard[i][j][0];m++){
					if(k==VVBoard[i][j][2*m+1]){
						ind[count]= VVBoard[i][j][2*m+2];
						val[count]= 1.0;
						count++;
					}
				}

			}
			if(count!=0){
				  error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1.0, NULL);
				  if (error) {
					  printf("Error: the command was not executed because of the following error in Gurobi:\n");
					  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					  GRBfreemodel(model);
					  GRBfreeenv(env);
					  freeValidValuesBoard(VVBoard);
					  free(obj);
					  free(vtype);
					  free(ind);
					  free(sol);
					  free(val);
					  return -1;
				  }
			}
 		  }
 	  }
  }

  /* Each value must appear once in each row */
  for(k=1;k<dim+1;k++){
	  for(j=0;j<dim;j++){
		  count=0;
		  for(i=0;i<dim;i++){
			  if(VVBoard[i][j]!=NULL){
				  for(m=0;m<VVBoard[i][j][0];m++){
								if(k==VVBoard[i][j][2*m+1]){
									ind[count]= VVBoard[i][j][2*m+2];
									val[count]= 1.0;
									count++;
								}
							}
			  }
 	  }
		  if(count!=0){
			  error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1.0, NULL);
					  if (error) {
						  printf("Error: the command was not executed because of the following error in Gurobi:\n");
						  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
						  GRBfreemodel(model);
						  GRBfreeenv(env);
						  freeValidValuesBoard(VVBoard);
						  free(obj);
						  free(vtype);
						  free(ind);
						  free(sol);
						  free(val);
						  return -1;
					  }
		  }
  }
  }
  /* Each value must appear once in each column */
  for(k=1;k<dim+1;k++){
	  for(i=0;i<dim;i++){
		  count=0;
		  for(j=0;j<dim;j++){
			  if(VVBoard[i][j]!=NULL){
				  for(m=0;m<VVBoard[i][j][0];m++){
								if(k==VVBoard[i][j][2*m+1]){
									ind[count]= VVBoard[i][j][2*m+2];
									val[count]= 1.0;
									count++;
								}
							}
			  }
 	  }
		  if(count!=0){
			  error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1.0, NULL);
					  if (error) {
						  printf("Error: the command was not executed because of the following error in Gurobi:\n");
						  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
						  GRBfreemodel(model);
						  GRBfreeenv(env);
						  freeValidValuesBoard(VVBoard);
						  free(obj);
						  free(vtype);
						  free(ind);
						  free(sol);
						  free(val);
						  return -1;
					  }
		  }
  }
  }

  /*constraints for each value in each block- each value must appear exactly once in each block*/
  for (k = 1; k < dim+1; k++) {
    for (ib = 0; ib < coldim; ib++) {
      for (jb = 0; jb < rowdim; jb++) {
        count = 0;
        for (i = ib*rowdim; i < (ib+1)*rowdim; i++) {
          for (j = jb*coldim; j < (jb+1)*coldim; j++) {
			  if (VVBoard[i][j]!=NULL){

				  for(m=0;m<VVBoard[i][j][0];m++){/*go over cell's valid vals and check if v is one of them. if it is add it to the constraint*/
					  if(VVBoard[i][j][2*m+1]==k){
							  ind[count]=VVBoard[i][j][2*m+2];/*Variable's number*/
							  val[count]= 1.0;
							  count++;

					  }
				  }
			  }
          }
        }
		if(count!=0){
			error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("Error: the command was not executed because of the following error in Gurobi:\n");
				printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				  GRBfreemodel(model);
				  GRBfreeenv(env);
				  freeValidValuesBoard(VVBoard);
				  free(obj);
				  free(vtype);
				  free(ind);
				  free(sol);
				  free(val);
				  return -1;
			}
		}
      }
    }
  }

  /* Optimize model - need to call this before calculation */
  error = GRBoptimize(model);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }

  /* Write model to 'mip1.lp' - this is not necessary but very helpful */
  error = GRBwrite(model, "mip1.lp");
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }

  /* Get solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }
  

  /* solution found */
  if (optimstatus == GRB_OPTIMAL) {

   /* get the objective -- the optimal result of the function */
   error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
   if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
 	  printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
 	  GRBfreemodel(model);
 	  GRBfreeenv(env);
 	  freeValidValuesBoard(VVBoard);
 	  free(obj);
 	  free(vtype);
 	  free(ind);
 	  free(sol);
 	  free(val);
 	  return -1;
   }

  /* get the solution - the assignment to each variable */
  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, totalNumOfVars, sol);
  if (error) {
	  printf("Error: the command was not executed because of the following error in Gurobi:\n");
	  printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  freeValidValuesBoard(VVBoard);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(sol);
	  free(val);
	  return -1;
  }
	  guess_hint_GRB(sol,x,y,threshold,command,VVBoard);
	  ret=1;

  }
  /* no solution found */
  else if (optimstatus == GRB_INF_OR_UNBD) {
	  printf("Error: the command can not be executed because the board is unsolvable.\n");
    ret=0;
  }
  /* error or calculation stopped */
  else {
	printf("Error: the command was not executed because of the following error in Gurobi:\n");
    printf("Optimization was stopped early\n");
    ret=0;
  }

  /* IMPORTANT !!! - Free model and environment */
  GRBfreemodel(model);
  GRBfreeenv(env);
  freeValidValuesBoard(VVBoard);
  free(obj);
  free(vtype);
  free(ind);
  free(sol);
  free(val);
  return ret;
}
