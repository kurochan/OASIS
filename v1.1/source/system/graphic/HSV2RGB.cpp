
void HSV2RGB( double *rr, double *gg, double *bb, double H, double S, double V )
{
    int in;
    double fl;
    double m, n;
    in = (int)( H / 60 );
    fl = ( H / 60 ) - in;
    if( !(in & 1)) fl = 1 - fl; // if i is even

    m = V * ( 1 - S );
    n = V * ( 1 - S * fl );
    switch( in ){
       case 0: *rr = V; *gg = n; *bb = m; break;
       case 1: *rr = n; *gg = V; *bb = m; break;
       case 2: *rr = m; *gg = V; *bb = n; break;
       case 3: *rr = m; *gg = n; *bb = V; break;
       case 4: *rr = n; *gg = m; *bb = V; break;
       case 5: *rr = V; *gg = m; *bb = n; break;
    }
	return ;
}
