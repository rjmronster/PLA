/* Purpose: test the Pereptron Learning Algo */

typedef enum boolean {
   true,
   false
} boolean;

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>  
#include <stdarg.h>
#include <math.h>

int main( int argc, char **argv ) ;

int main( int argc, char **argv ) {

   char ot1_file[1024] = {""};
   char ot2_file[1024] = {""};
   char ot3_file[1024] = {""};
   char ot4_file[1024] = {""};
   char ot5_file[1024] = {""};

   FILE *fp_ot1 = NULL;
   FILE *fp_ot2 = NULL;
   FILE *fp_ot3 = NULL;
   FILE *fp_ot4 = NULL;
   FILE *fp_ot5 = NULL;

   boolean have_plus = true;
   boolean have_minus = true;
   boolean same_seed = false;
   struct timeval tt;
   struct timezone tz;
   boolean first = true;
   int i;
   int j;
   int nsamps = 0;
   int iterate;
   double rnd_x;
   double rnd_y;
   double rnd_x1;
   double rnd_y1;
   double rnd_x2;
   double rnd_y2;
   double slope;
   double bias;
   double slopeg = 0.0;
   double biasg = 0.0;
   double di;
   double y;
   double **training;
   double *weights;
   double *weights_old;
   double dot;

   int debug = 0x00;

   if ( argc == 1 || *(*(argv+1)+1) == 'h' || *(*(argv+1)+1) == 'H' ) {
       goto default1;
   }
  

   /* Get use input. */
   while( --argc > 0 ) {
      ++argv;                   /* look at the next argv, skip argv[0] */
      switch( *(*argv+1) ) {    /* and only at the second character */
         case 'S':
            same_seed = true;
            break;

         case 'n':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            if ( sscanf( *argv, "%d", &nsamps ) != 1 ) {; 
               fprintf( stderr, "can't read nsamps\n"
                                "nsamps = %s\n", *argv );
               exit(-1);
            }
            break;

         case 'i':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            if ( sscanf( *argv, "%d", &iterate ) != 1 ) {; 
               fprintf( stderr, "can't read iterate\n"
                                "iterate = %s\n", *argv );
               exit(-1);
            }
            break;

         case 'o':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            strcpy( ot1_file, *argv );
            if( (fp_ot1 = fopen(ot1_file, "w")) == NULL ) {
               fprintf( stderr, "Could not open output file %s\n", ot1_file);
               exit(-1);
            }
            break;

         case 'p':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            strcpy( ot2_file, *argv );
            if( (fp_ot2 = fopen(ot2_file, "w")) == NULL ) {
               fprintf( stderr, "Could not open output file %s\n", ot2_file);
               exit(-1);
            }
            break;

         case 'q':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            strcpy( ot3_file, *argv );
            if( (fp_ot3 = fopen(ot3_file, "w")) == NULL ) {
               fprintf( stderr, "Could not open output file %s\n", ot3_file);
               exit(-1);
            }
            break;

         case 'r':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            strcpy( ot4_file, *argv );
            if( (fp_ot4 = fopen(ot4_file, "w")) == NULL ) {
               fprintf( stderr, "Could not open output file %s\n", ot4_file);
               exit(-1);
            }
            break;

         case 's':
            ++argv; --argc;
            if ( argc == 0 ) goto default1;
            strcpy( ot5_file, *argv );
            if( (fp_ot5 = fopen(ot5_file, "w")) == NULL ) {
               fprintf( stderr, "Could not open output file %s\n", ot5_file);
               exit(-1);
            }
            break;

         default:
            default1:
            printf("\ntest the Pereptron Learning Algo\n" );
            printf("\n"
                   "Use: PLA [ -S use same seed]\n"  
                   "           -n number_of_samples\n"
                   "           -i number_of_iterations\n"
                   "           -o output_rpts_1.txt\n"
                   "           -p output_line_2.txt\n"
                   "           -r output_plus_3.txt\n"
                   "           -q output_mins_4.txt\n"
                   "           -s output_targ_5.txt\n"
                   "\n" );
            return(0);
      }
   } /* endwhile */
   if ( nsamps == 0 ) {
      fprintf( stderr, "\n *** enter the number of samples with -n\n");
      goto default1;
   }

   if ( first ) {
      first = false;
      if ( ! same_seed ) {
         gettimeofday(&tt, &tz);
         srand( tt.tv_usec );
      }
   }
   rnd_x1 =  2.0*((double)rand()) / RAND_MAX - 1.0;
   rnd_y1 =  2.0*((double)rand()) / RAND_MAX - 1.0;
   rnd_x2 =  2.0*((double)rand()) / RAND_MAX - 1.0;
   rnd_y2 =  2.0*((double)rand()) / RAND_MAX - 1.0;
   slope = (rnd_y2 - rnd_y1) / (rnd_x2 - rnd_x1);
   bias  = rnd_y2 - rnd_x2*slope;
   fprintf( fp_ot1, "%12.8f %12.8f\n\n", rnd_x1, rnd_y1 );
   fprintf( fp_ot1, "%12.8f %12.8f\n\n", rnd_x2, rnd_y2 );
   for ( i = 0; i < 201; i++ ) {
      di = (double) i / 100 - 1.0;
      fprintf( fp_ot2, "%12.8f %12.8f\n", di, slope*di + bias );
   }
   
   training = ( double * * ) malloc( 3 * sizeof( double *) );
   training[0] = (double *) malloc( 3*nsamps * sizeof(double) );
   for ( i = 1; i < 3; i++ ) training[i] = training[0] + i*nsamps;
   weights = ( double * ) malloc ( 3 * sizeof(double)) ;
   weights[0]  = weights[1] = weights[2] = 0;
   weights_old = ( double * ) malloc ( 3 * sizeof(double)) ;
   weights_old[0]  = weights_old[1] = weights_old[2] = 0;
repeatit:
   have_plus = false;
   have_minus = false;
   for ( i = 0; i < nsamps; i++ ) {
      /* generate RV [-1,1] */
      rnd_x =  2.0*((double)rand()) / RAND_MAX - 1.0;
      rnd_y =  2.0*((double)rand()) / RAND_MAX - 1.0;
      y = rnd_x*slope + bias;
      training[0][i] = rnd_x;
      training[1][i] = rnd_y;
      if ( y <= rnd_y  ) {
         fprintf( fp_ot3, "%12.8f %12.8f\n", rnd_x, rnd_y );
         training[2][i] =  1.0;
         have_plus = true;
      }
      if ( y > rnd_y  ) {
         fprintf( fp_ot4, "%12.8f %12.8f\n", rnd_x, rnd_y );
         training[2][i] = -1.0;
         have_minus = true;
      }
   }
   if ( have_plus == false || have_minus == false ) goto repeatit;
   for ( j = 0; j < iterate; j++ ) {
      for ( i = 0; i < nsamps; i++ ) {
         dot = weights[0] + weights[1]*training[0][i] + weights[2]*training[1][i];
         if ( (dot >= 0 && training[2][i] < 0) || (dot < 0  && training[2][i] > 0) ) {
            weights[0] += training[2][i]*1.0; 
            weights[1] += training[2][i]*training[0][i];
            weights[2] += training[2][i]*training[1][i];
            break;
         }
      }
      if ( weights[2] != 0.0 ) {
        slopeg = -weights[1]/weights[2];
        biasg  = -weights[0]/weights[2];
      } else {
        slopeg = 0.0;
        biasg  = 0.0;
      }
      fprintf( stderr, "iterate %3d weights: %12.8f %12.8f %12.8f slope %12.8f splopg %12.8f bias %12.8f biasg %12.8f\n", 
                j, weights[0], weights[1], weights[2], slope, slopeg, bias, biasg );
      if ( weights_old[0] == weights[0] && weights_old[1] == weights[1] && weights_old[2] == weights[2] ) {
         fprintf( stderr, "converged after %3d times\n", j );
         break;
      }
      weights_old[0] = weights[0];
      weights_old[1] = weights[1];
      weights_old[2] = weights[2];
   }
   for ( i = 0; i < 201; i++ ) {
      di = (double) i / 100 - 1.0;
      fprintf( fp_ot5, "%12.8f %12.8f\n", di, slopeg*di + biasg );
   }
   return(0);
}
