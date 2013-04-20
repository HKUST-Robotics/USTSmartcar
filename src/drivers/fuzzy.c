
#include "fuzzy.h"

/*unsigned int rules[RULE_TOT]=
{
      // if.    and.    then.
	a_LB,   e_LB,   LB,
	a_LB,   e_L,    LB,
	a_LB,   e_M,    LB,
	a_LB,   e_R,    LM,
	a_LB,   e_RB,   LM,

	a_LM,   e_LB,   LB,
	a_LM,   e_L,    LB,
	a_LM,   e_M,    LM,
	a_LM,   e_R,    LM,
	a_LM,   e_RB,   LM,

	a_LS,   e_LB,   LM,
	a_LS,   e_L,    LM,
	a_LS,   e_M,    LM,
	a_LS,   e_R,    LS,
	a_LS,   e_RB,   LS,

	a_M,    e_LB,   F,
	a_M,    e_L,    F,
	a_M,    e_M,    F,
	a_M,    e_R,    F,
	a_M,    e_RB,   F,
	
	a_RS,   e_LB,   RS,
	a_RS,   e_L,    RS,
	a_RS,   e_M,    RS,
	a_RS,   e_R,    RM,
	a_RS,   e_RB,   RM,
};

char input_memf[input_total][mf_total][4]=
{
       //slope
       {     //point1, slope1,point3,slope2

		{ 0  , 0, 32 , 10 },         // LB
		{ 35 , 10, 65 , 11 },        // LM
		{ 65 , 11, 92 , 9 },         // LS
		{ 92 , 9, 130, 9 },          // M
		{ 130, 9, 167, 10 },         // RS
		{ 167, 10, 199, 10 },        // RM
		{ 199, 10, 255, 0 }          // RB
       },
       
       //y-intercept
	{     //point1, slope1,point3,slope2
		{ 0, 0, 39, 7 },            // e_RB
		{ 39, 7, 84, 9},            // e_R
		{ 84, 9, 142, 9 },          // M
		{ 142, 9, 187, 7 },         // e_L
		{ 187, 7, 255,0}            //e_LB
         }
};

void defuzzify() 
{
    long numerator, denominator;
    unsigned int j;
	numerator=0; // ?????
        denominator=0;
	for (j=0; j<MF_TOT; j++) 
          {   numerator+=(outputs[j]*output_memf[j]);
	      denominator+=outputs[j];
              outputs[j]=0;  }  
        if (denominator)
          fuzzy_out=numerator/denominator; 
	else 
	   fuzzy_out=DEFAULT_VALUE; 	
}

int compute_mmval(unsigned int inp_num,unsigned int label) 
{
	if (input[inp_num] < input_memf[inp_num][label][0]) 
            return 0;
        else 
           { if (input[inp_num] < input_memf[inp_num][label][2]) 
               {   temp=input[inp_num]; 
		   temp-=input_memf[inp_num][label][0];
		   { if (!input_memf[inp_num][label][1]) 
                         temp=MU_MAX;
		     else 
                         temp*=input_memf[inp_num][label][1];  }
                   { if (temp < 233) 
			 return temp; 
		     else 
                         return MU_MAX;   }   
                 }
              else 
                 {   temp=input[inp_num]; 
		     temp-=input_memf[inp_num][label][2];
	             temp*=input_memf[inp_num][label][3];
		     temp=MU_MAX-temp;
		     if (temp < 0) 
			return 0; 
		     else 
			return temp; 
		  }
	    }
	 return 0;
}

void fuzzy_engine() {
	char then; 
	unsigned int if_val, clause, 	mu, inp_num, label; 
	then=0; 
	if_val=MU_MAX; 

	for (clause=0; clause<RULE_TOT; clause++) 
           {   clause_val=rules[clause];
               if (!(clause_type)) 
                 {  if (then) 
                       {  then=0;
			  if_val=MU_MAX;  }
		    inp_num=clause_val & IO_NUM; 
		    label=(clause_val & LABEL_NUM) / 16; 
	            mu=compute_mmval(inp_num, label);
		    
                    if  (!mu)
                        {
                            do 
                            {   	clause++;
				}while (clause <RULE_TOT && !(rules[clause]&0x80));
				//????
				while (clause+1<RULE_TOT && (rules[clause+1]&0x80)){
				
	clause++;
				}
				if_val=MU_MAX;
			}
			else{
				if(mu<if_val){
					if_val=mu;}
			}
		}
			
		 else { 
			then=1; 
		
			if (outputs[(clause_val & 0x70)/16] < if_val) {
				outputs[(clause_val & 0x70)/16]=if_val;
			}
		 }
	}
	defuzzify(); 

}*/
