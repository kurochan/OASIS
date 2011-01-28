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
	double dist(double x, double y, double z);
	void setlocalmx( double LGmpx, double LGmpy, double LGmpz,  double LGm00, double LGm10, double LGm20,  double LGm01, double LGm11, double LGm21,  double LGm02, double LGm12, double LGm22 );
	void setcamx( double cpx, double cpy, double cpz,  double ppx, double ppy, double ppz, double ppv );
	void setcam(double cx=0, double cy=0, double cz=0,  double tx=0, double ty=0, double tz=0, bool flag=1 ){setcamx(cx,cy,cz,tx,ty,tz,flag);};
	void trans( double &inx, double &iny, double &inz,  int &oux, int &ouy, double &ouz, int &ouf) ;
	void vpos(  double v01, double v02, double v03) ;
	void line( double ppx, double ppy, double ppz,  double ssx, double ssy, double ssz) ;
	void arrow( double v11, double v12, double v13,  double v14, double v15, double v16 );
	void box( double v11, double v12, double v13,  double v14, double v15, double v16);
	void circle( double ppx, double ppy, double ppz, double ppr, int ppf=0 );
	void mes( char *pmsg, double ppx, double ppy, double ppz ) ;
} ;

// d3dist 距離 (ベクトル) の絶対値を求める (x, y, z)
//-----------------------------------------------------------
inline double d3module::dist(double x, double y, double z){
	return sqrt(x * x + y * y + z * z);
}


// d3vpos 座標変換 (x, y, z) -> dx, dy, dz, df
//-----------------------------------------------------------
inline void d3module::vpos(  double v01, double v02, double v03) {

	//; bkup last-data
	ex = dx ;
	ey = dy ;
	ef = df ;

	trans( v01, v02, v03, dx, dy, dz, df ) ;

	return ;
}


// d3trans 座標変換 macro (inx, iny, inz,  oux, ouy, ouz, ouf)
//-----------------------------------------------------------
inline void d3module::trans( double &inx, double &iny, double &inz,  int &oux, int &ouy, double &ouz, int &ouf) {

	ouz = (LGSm01*inx + LGSm11*iny + LGSm21*inz + LGSmpy) ;
	ouf = 0 ;
	if (ouz > 0) {
		oux = wincx + (LGSm00*inx + LGSm10*iny + LGSm20*inz + LGSmpx) / ouz ;
		ouy = wincy - (LGSm02*inx + LGSm12*iny + LGSm22*inz + LGSmpz) / ouz ;
		ouf=1;
		if (oux/8000 || ouy/8000)
			ouf = 0 ;
	}

	return ;
}


