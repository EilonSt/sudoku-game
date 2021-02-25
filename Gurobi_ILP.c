/*building a model for Gurobi optimization using the ILP method*/

#include <stdlib.h>
#include <stdio.h>
#include "GurobiAux.h"
#include "Board.h"
#include "gurobi_c.h"

extern Board * board;

/*	Tries to find a solution to the board using ILP.
 *  If isValidate==1, then the caller is the "Validate" command, so we don't need the solution itself,
 *  we only need to know if it exists or not.
 *  Otherwise, the solution (if it exists) will be saved in the game board.
 *  The function returns 1 if a solution was found, 0 if there is no solution, -1 if an error occurred
 * */
char gurobiILP(char isValidate)
{
	/*general variables*/
	char ret;
	int i,j,k,v,ib,jb;
	int totalNumOfVars;/*number of variables required for the Gurobi program*/
	int numOfValidVals;/*number of valid variables for a cell*/
	int count=0;
	int size=board->size;
	int rowsInBlock=board->rowsinblock;
	int colsInBlock=board->colsinblock;
	int *** validValsBoard;/*each cell holds an array of the cell's valid values considering the existing cells*/

	/*Gurobi variables*/
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int error = 0;
	int * ind;
	double * val;
	double * obj;	 /*objective function*/
	char * vtype;
	int optimstatus;
	double *   sol;	    /* values of the varibals if solution has been found*/

	/*dynamically allocate memory*/
	validValsBoard=(int***)malloc(size*sizeof(int**));
	if (validValsBoard==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}

	/*count number of required variables + set valid values board*/
	totalNumOfVars=validValuesBoard(validValsBoard);

		/*dynamically allocate memory*/
		obj=(double*)malloc(sizeof(double)*totalNumOfVars);
		if (obj==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}

		vtype=(char*)malloc(totalNumOfVars*sizeof(char));
		if (vtype==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
		ind=(int*)malloc(size*sizeof(int));
		if (ind==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
		val=(double*)malloc(size*sizeof(double));
		if (val==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
		sol=(double*)malloc(sizeof(double)*totalNumOfVars);
			if (sol==NULL){
				printf("Error: failed to allocate memory.\n");
				exit(1);
			}
		/*Setting objective coefficients.
		 * Doesn't matter which objective function because we're only looking for a solution that answers all of the constraints.
		 * */
		for(i=0;i<totalNumOfVars;i++){
			obj[i]=0.0;
		}
		obj[1]=1.0;

		/*Setting all variables types to binary*/
		  for(i=0;i<totalNumOfVars;i++){
			  vtype[i]=GRB_BINARY;
		  }

		/*Initializing all values of the constraint coefficients to 1*/
		for(i=0;i<size;i++){
			val[i]=1.0;
		}

		/* Create environment - log file is mip1.log */
		error = GRBloadenv(&env, "mip1.log");
		if (error) {
			printf("Error: the command was not executed because of the following error in Gurobi:\n");
			printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
			  GRBfreemodel(model);
			  GRBfreeenv(env);
			  free(obj);
			  free(vtype);
			  free(ind);
			  free(val);
			  free(sol);
			  freeValidValuesBoard(validValsBoard);
			  return -1;
		}

	  /*Don't print output to console*/
	  error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* Create an empty model named "mip1" */
	  error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* add variables to model */
	  error = GRBaddvars(model, totalNumOfVars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* Change objective sense to maximization */
	  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* update the model - to integrate new variables */
	  error = GRBupdatemodel(model);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /*CONSTRAINTS*/

	  /*constraints for each cell - each cell must have exactly one value*/
	  for (i=0;i<size;i++){
		  for (j=0;j<size;j++){
			  if (validValsBoard[i][j]!=NULL){
				  numOfValidVals=validValsBoard[i][j][0];
				  for(k=0;k<numOfValidVals;k++){
					  ind[k]=validValsBoard[i][j][2*k+2];/*cell 0 holds number of valid values. All other even cells hold the variable number*/
				  }
				  error = GRBaddconstr(model, numOfValidVals, ind, val, GRB_EQUAL, 1, NULL);
				  if (error) {
					  printf("Error: the command was not executed because of the following error in Gurobi:\n");
					  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					  GRBfreemodel(model);
					  GRBfreeenv(env);
					  free(obj);
					  free(vtype);
					  free(ind);
					  free(val);
					  free(sol);
					  freeValidValuesBoard(validValsBoard);
					  return -1;
				  }
			  }
		  }
	  }

	  /*constraints for each value in each row - each value must appear exactly once in each row*/
	  for(i=0;i<size;i++){
		  for(v=1;v<size+1;v++){
			  /*constraint for row i value v*/
			  count=0;/*counts number of variables in the constraint*/
			  for(j=0;j<size;j++){
				  if (validValsBoard[i][j]!=NULL){
					  numOfValidVals=validValsBoard[i][j][0];
					  for(k=0;k<numOfValidVals;k++){/*go over cell's valid vals and check if v is one of them. if it is add it to the constraint*/
						  if(validValsBoard[i][j][2*k+1]>=v){
							  if(validValsBoard[i][j][2*k+1]==v){
								  ind[count]=validValsBoard[i][j][2*k+2];/*Variable's number*/
								  count+=1;
							  }
							  break;
						  }
					  }
				  }
			  }
			  if(count!=0){
				  error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1, NULL);
				  if (error) {
					  printf("Error: the command was not executed because of the following error in Gurobi:\n");
					  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					  GRBfreemodel(model);
					  GRBfreeenv(env);
					  free(obj);
					  free(vtype);
					  free(ind);
					  free(val);
					  free(sol);
					  freeValidValuesBoard(validValsBoard);
					  return -1;
				  }
			  }
			  /*otherwise v already exists in some cell in this row*/
		  }
	  }

	  /*constraints for each value in each column - each value must appear exactly once in each column*/
	  for(j=0;j<size;j++){
		  for(v=1;v<size+1;v++){
			  /*constraint for column j value v*/
			  count=0;/*counts number of variables in the constraint*/
			  for(i=0;i<size;i++){
				  if (validValsBoard[i][j]!=NULL){
					  numOfValidVals=validValsBoard[i][j][0];
					  for(k=0;k<numOfValidVals;k++){/*go over cell's valid vals and check if v is one of them. if it is add it to the constraint*/
						  if(validValsBoard[i][j][2*k+1]>=v){
							  if(validValsBoard[i][j][2*k+1]==v){
								  ind[count]=validValsBoard[i][j][2*k+2];/*Variable's number*/
								  count+=1;
							  }
							  break;
						  }
					  }
				  }
			  }
			  if(count!=0){
				  error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1, NULL);
				  if (error) {
					  printf("Error: the command was not executed because of the following error in Gurobi:\n");
					  printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					  GRBfreemodel(model);
					  GRBfreeenv(env);
					  free(obj);
					  free(vtype);
					  free(ind);
					  free(val);
					  free(sol);
					  freeValidValuesBoard(validValsBoard);
					  return -1;
				  }
			  }
			  /*otherwise v already exists in some cell in this row*/
		  }
	  }

	  /*constraints for each value in each block- each value must appear exactly once in each block*/
	  for (v = 1; v < size+1; v++) { /*value v*/
	    for (ib = 0; ib < colsInBlock; ib++) {
	      for (jb = 0; jb < rowsInBlock; jb++) {
	        count = 0;
	        for (i = ib*rowsInBlock; i < (ib+1)*rowsInBlock; i++) {
	          for (j = jb*colsInBlock; j < (jb+1)*colsInBlock; j++) {
				  if (validValsBoard[i][j]!=NULL){
					  numOfValidVals=validValsBoard[i][j][0];
					  for(k=0;k<numOfValidVals;k++){/*go over cell's valid vals and check if v is one of them. if it is add it to the constraint*/
						  if(validValsBoard[i][j][2*k+1]>=v){
							  if(validValsBoard[i][j][2*k+1]==v){
								  ind[count]=validValsBoard[i][j][2*k+2];/*Variable's number*/
								  count+=1;
							  }
							  break;
						  }
					  }
				  }
	          }
	        }
			if(count!=0){
				error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1, NULL);
				if (error) {
					printf("Error: the command was not executed because of the following error in Gurobi:\n");
					printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					  GRBfreemodel(model);
					  GRBfreeenv(env);
					  free(obj);
					  free(vtype);
					  free(ind);
					  free(val);
					  free(sol);
					  freeValidValuesBoard(validValsBoard);
					return -1;
				}
			}
	      }
	    }
	  }


	  /* Optimize model */
	  error = GRBoptimize(model);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* Write model to 'mip1.lp'*/
	  error = GRBwrite(model, "mip1.lp");
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* Get solution information */
	  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	  if (error) {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }

	  /* solution found */
	  if (optimstatus == GRB_OPTIMAL) {
		  ret=1;
		  if(!isValidate){
			  /* get the solution - the assignment to each variable */
			  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, totalNumOfVars, sol);
			  if (error) {
				  printf("Error: the command was not executed because of the following error in Gurobi:\n");
				  printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
				  GRBfreemodel(model);
				  GRBfreeenv(env);
				  free(obj);
				  free(vtype);
				  free(ind);
				  free(val);
				  free(sol);
				  freeValidValuesBoard(validValsBoard);
				  return -1;
			  }
			  for (i=0;i<size;i++){
				  for (j=0;j<size;j++){
					  if (validValsBoard[i][j]!=NULL){
						  numOfValidVals=validValsBoard[i][j][0];
						  for(k=0;k<numOfValidVals;k++){
							  if(sol[validValsBoard[i][j][2*k+2]]==1){
								  board->cells[i][j].val=validValsBoard[i][j][2*k+1];
								  board->cells[i][j].error=0;
								  board->cells[i][j].fixed=0;
							  }
						  }
					  }
				  }
			  }
		  }
	  }
	  /* no solution found */
	  else if (optimstatus == GRB_INF_OR_UNBD) {
	    ret=0;
	  }
	  /* error or calculation stopped */
	  else {
		  printf("Error: the command was not executed because of the following error in Gurobi:\n");
		  printf("Error: Optimization was stopped early\n");
		  GRBfreemodel(model);
		  GRBfreeenv(env);
		  free(obj);
		  free(vtype);
		  free(ind);
		  free(val);
		  free(sol);
		  freeValidValuesBoard(validValsBoard);
		  return -1;
	  }
	  /* Free model, environment and other dynamically allocated memory*/
	  GRBfreemodel(model);
	  GRBfreeenv(env);
	  free(obj);
	  free(vtype);
	  free(ind);
	  free(val);
	  free(sol);
	  freeValidValuesBoard(validValsBoard);

	return ret;
}
