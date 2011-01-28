#include <math.h>
#include "unsigned.h"
#include "kernel.h"

void HSV2RGB( UCHAR &R, UCHAR &G, UCHAR &B, UCHAR H, UCHAR S, UCHAR V )
{
	UINT uiH=(H*360)/191;

	if (S==0){
		R=V;
		G=V;
		B=V;
		return ;
	}


	UCHAR I = floor( uiH / 60 );
	UINT F = uiH - I*60 ;
	UINT uiM = (255-S)*V, uiN = (255*60-S*F)*V, uiK = (255*60-S*(60-F))*V ;
	UCHAR M = uiM/255, N = uiN/(255*60), K = uiK/(255*60) ;

	switch(I){
		case 0: R=V; G=K; B=M; break ;
		case 1: R=N; G=V; B=M; break ;
		case 2: R=M; G=V; B=K; break ;
		case 3: R=M; G=N; B=V; break ;
		case 4: R=K; G=M; B=V; break ;
		case 5: R=V; G=M; B=N; break ;
	}
	return ;
}
