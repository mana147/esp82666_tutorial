





 signed int Get_tb(signed int *mang,unsigned int number )
 {
    long value=0;
    signed int  data=0;
    for(unsigned int i=0;i<number;i++)
    {
      value+=mang[i];
    }
    data=value/number;
    return  data;
 }


signed int Lay_trung_binh (unsigned int number_size,signed int *buffer,signed int data,unsigned char ch)
{
  signed int  data_out=0;
  signed int  tb;
  
  for(unsigned int i=0;i<number_size-1;i++)
   {
     buffer[i] = buffer[i+1];
   }
   data_out=Get_tb(buffer,number_size);
   tb= data_out-data;
   if(tb<0)
   {
     tb=-tb;
   }   
   if(ch)
   {
     if(tb>50)
     {
       buffer[number_size-1] = data_out;
     }
     else
     {
       buffer[number_size-1] = data;
     }
   }
     else
   {
     buffer[number_size-1] = data;
   }
   return  data_out;
}


signed int kalman_filter(signed int z, float N_R, float N_Q,unsigned char k)
{
 const  double H =N_R*N_R;
 const  double Q =N_Q*N_Q; 
 static double x_hat[10],P[10];
 float P_,K;
 
    P_ = P[k] + Q;                
    K = P_/(P_ + H);               
    x_hat[k] = x_hat[k] + K*(z - x_hat[k]); 
    P[k] = ( 1 - K)*P_ ;             
    
 return (signed int)x_hat[k];
}


float LPF(float x, float CUTOFF,float SAMPLE_RATE)
{
  float RC, dt, alpha, y;
  static float ylast=0;
  RC = 1.0/(CUTOFF*2*3.14);
  dt = 1.0/SAMPLE_RATE;
  alpha = dt/(RC+dt);
  y = ylast + alpha * ( x - ylast ); 
  ylast = y;
  return y;
}


float HPF(float x, float CUTOFF,float SAMPLE_RATE)
{
  float RC = 1.0/(CUTOFF*2*3.14);
  float dt = 1.0/SAMPLE_RATE;
  float alpha = RC/(RC+dt);
  float y;
  static float xlast=0, ylast=0;
  y = alpha * ( ylast + x - xlast); 
  ylast = y;
  xlast = x;
  return y;
}
