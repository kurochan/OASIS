/*
 *  d3module.h
 *  
 *
 *  Created by Liva on 10/12/17.
 *
 */

//============================================================
//                                                 2008/05/01
//	S.Programs d3module 1.67
//	Simple, Stupid 3D module for HSP3
//                               http://sprocket.babyblue.jp/
//============================================================

class d3module {
	HSP &_hsp;
	double GSm00, GSm01, GSm02, GSm10, GSm11, GSm12, GSm21, GSm22, GSmpx, GSmpy, GSmpz ;
	double LGSm00, LGSm01, LGSm02, LGSm10, LGSm11, LGSm12, LGSm20, LGSm21, LGSm22, LGSmpx, LGSmpy, LGSmpz ;
	double ax, ay, az, bx, by, bz, cx, cy, cz ;
	double r0, r1, sin0, sin1, cos0, cos1;
	int dx, dy, &df, ex, ey, ef, af ; 
	double dz, ez;
	short wincx, wincy ;
public:
	d3module(HSP &hsp);
	void setlocalmx( double LGmpx, double LGmpy, double LGmpz,  double LGm00, double LGm10, double LGm20,  double LGm01, double LGm11, double LGm21,  double LGm02, double LGm12, double LGm22 );
	void setcamx( double cpx, double cpy, double cpz,  double ppx, double ppy, double ppz, double ppv );
	void setcam(double cx, double cy, double cz,  double tx, double ty, double tz );
	void trans( double &inx, double &iny, double &inz,  int &oux, int &ouy, double &ouz, int &ouf) ;
	void vpos(  double v01, double v02, double v03) ;
	void line( double ppx, double ppy, double ppz,  double ssx, double ssy, double ssz) ;
	void arrow( double v11, double v12, double v13,  double v14, double v15, double v16 );
	void box( double v11, double v12, double v13,  double v14, double v15, double v16);
	void circle( double ppx, double ppy, double ppz, double ppr, int ppf );
	void mes( char *pmsg, double ppx, double ppy, double ppz ) ;
} ;

