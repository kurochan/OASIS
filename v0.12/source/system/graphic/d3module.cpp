/*
 *  d3module.cpp
 *  
 *
 *  Created by Liva on 10/12/17.
 *
 */

#include "kernel.h"
#include <math.h>


d3module::d3module(HSP &hsp) :_hsp(hsp), df(_hsp.stat) {
	dx=0;
	dy=0;
	dz=0;
	ex=0;
	ey=0;
	ez=0;
}


// setlocalmx ���[�J�����W�n�ݒ� ���s�ړ��� + 3x3 �ό`�}�g���N�X (px, py, pz,  m00, m01, m02,  m10, m11, m12,  m20, m21, m22)
//-----------------------------------------------------------
void d3module::setlocalmx( double LGmpx, double LGmpy, double LGmpz,  double LGm00, double LGm10, double LGm20,  double LGm01, double LGm11, double LGm21,  double LGm02, double LGm12, double LGm22 ){

	// Local->Global->Screen Matrix Setup
	// ���W�ϊ����Z�p�}�g���N�X�ݒ� (Local->Global Matrix �� Global->Screen Matrix ���������Z)

	LGSm00 = GSm00*LGm00 + GSm10*LGm01 ; //+ GSm20*LGm02 // GSm20 == 0
	LGSm10 = GSm00*LGm10 + GSm10*LGm11 ; //+ GSm20*LGm12
	LGSm20 = GSm00*LGm20 + GSm10*LGm21 ; //+ GSm20*LGm22
	LGSmpx = GSm00*LGmpx + GSm10*LGmpy + GSmpx ; //+ GSm20*LGmpz

	LGSm01 = GSm01*LGm00 + GSm11*LGm01 + GSm21*LGm02 ;
	LGSm11 = GSm01*LGm10 + GSm11*LGm11 + GSm21*LGm12 ;
	LGSm21 = GSm01*LGm20 + GSm11*LGm21 + GSm21*LGm22 ;
	LGSmpy = GSm01*LGmpx + GSm11*LGmpy + GSm21*LGmpz + GSmpy ;

	LGSm02 = GSm02*LGm00 + GSm12*LGm01 + GSm22*LGm02 ;
	LGSm12 = GSm02*LGm10 + GSm12*LGm11 + GSm22*LGm12 ;
	LGSm22 = GSm02*LGm20 + GSm12*LGm21 + GSm22*LGm22 ;
	LGSmpz = GSm02*LGmpx + GSm12*LGmpy + GSm22*LGmpz + GSmpz ;

	return ;
}


// setcamx �J�����ʒu�ݒ� (cx, cy, cz,  tx, ty, tz)
//-----------------------------------------------------------
void d3module::setcamx( double cpx, double cpy, double cpz,  double ppx, double ppy, double ppz, double ppv ){
	
	// ���W�ϊ����ʂ� stat �ɕԂ��悤�ɂ���
	//	mref df, 64 ;
	
	//�p�����[�^�ݒ�
	wincx = _hsp.ginfo_winx() / 2 ;
	wincy = _hsp.ginfo_winy() / 2 ;
	
	// �J���������O�p��v�Z
	ax = cpx - ppx ;
	ay = cpy - ppy ;
	az = cpz - ppz ;
	
	r0 = sqrt(ax*ax + ay*ay) ;
	r1 = sqrt(r0*r0 + az*az) ;
	
	if (r0 != 0.0) {
		cos0 = -ax / r0 ;
		sin0 = -ay / r0 ;
	}
	
	if (r1 != 0.0) {
		cos1 = r0 / r1 ;
		sin1 = az / r1 ;
	}
	
	// Global->Screen Matrix Setup
	// �O���[�o�����W �� �X�N���[�����W �ϊ��}�g���N�X
	
	az = ppv / (0.01 + _hsp.ginfo_winy()) ; //����p
	
	GSm00 = sin0         ; GSm10 = -cos0       ;// : GSm20 =  0.0
	GSm01 = cos0*cos1*az ; GSm11 =  sin0*cos1*az ; GSm21 = -sin1*az ;
	GSm02 = cos0*sin1    ; GSm12 =  sin0*sin1    ; GSm22 =  cos1    ;
	
	GSmpx = -(GSm00*cpx + GSm10*cpy) ;
	GSmpy = -(GSm01*cpx + GSm11*cpy + GSm21*cpz) ;
	GSmpz = -(GSm02*cpx + GSm12*cpy + GSm22*cpz) ;
	
	
	// Local->Global->Screen Matrix Setup
	// ���W�ϊ����Z�p�}�g���N�X�ݒ� (Global->Screen Matrix �ŏ�����)
	
	setlocalmx( 0,0,0, 1,0,0, 0,1,0, 0,0,1 ) ;
	
	return ;
}

// setcam �J�����ʒu�ݒ� (cx, cy, cz,  tx, ty, tz)
// �p�����[�^�ȗ��p�}�N��
//-----------------------------------------------------------
void d3module::setcam( double cx=0, double cy=0, double cz=0,  double tx=0, double ty=0, double tz=0 ){
	setcamx(cx,cy,cz,tx,ty,tz,1);
}

#include <stdio.h>
// d3trans ���W�ϊ� macro (inx, iny, inz,  oux, ouy, ouz, ouf)
//-----------------------------------------------------------
void d3module::trans( double &inx, double &iny, double &inz,  int &oux, int &ouy, double &ouz, int &ouf) {

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


// d3vpos ���W�ϊ� (x, y, z) -> dx, dy, dz, df
//-----------------------------------------------------------
void d3module::vpos(  double v01, double v02, double v03) {

	//; bkup last-data
	ex = dx ;
	ey = dy ;
	ef = df ;

	trans( v01, v02, v03, dx, dy, dz, df ) ;

	return ;

}

// line ���C���`�� (x1, y1, z1,  x2, y2, z2) / �N���b�s���O����
//-----------------------------------------------------------
void d3module::line( double ppx, double ppy, double ppz,  double ssx, double ssy, double ssz) {

	vpos( ssx, ssy, ssz ) ; //, ex, ey, ez, ef
	vpos( ppx, ppy, ppz ) ; //, dx, dy, dz, df

	//; �N���b�v�s�v
	if (df & ef) {
		_hsp.line( (int)(dx+0.5), (int)(dy+0.5), (int)(ex+0.5), (int)(ey+0.5) ) ;
		return ;
	}

	//; �񕪒T���ŃN���b�s���O
	if (df | ef) {
		if (df) {
			_hsp.pos( (int)(dx+0.5), (int)(dy+0.5) );
			ax = ppx ; ay = ppy ; az = ppz ;
			bx = ssx ; by = ssy ; bz = ssz ;
		} else {
			_hsp.pos( (int)(ex+0.5), (int)(ey+0.5) ) ;
			ax = ssx ; ay = ssy ; az = ssz ;
			bx = ppx ; by = ppy ; bz = ppz ;
		}
		
		for(char cnt=0; cnt<10; cnt++) {
			cx = (ax + bx) / 2 ;
			cy = (ay + by) / 2 ;
			cz = (az + bz) / 2 ;
			trans( cx, cy, cz,  ex, ey, ez, af) ;
			if (af) {
				ax = cx ; ay = cy ; az = cz ;
				_hsp.line( (int)(ex+0.5), (int)(ey+0.5), _hsp.ginfo_cx(), _hsp.ginfo_cy() );
			} else {
				bx = cx ; by = cy ; bz = cz ;
			}
		}
	}

	return ;
}
	/*	
// d3arrow ����`�� (x1, y1, z1,  x2, y2, z2)
//-----------------------------------------------------------
void d3module::arrow( double v11, double v12, double v13,  double v14, double v15, double v16 ){

	line( v11, v12, v13,  v14, v15, v16) ;

	if (df & ef) {
		// vector of arrow
		double a = atan(dy-ey, dx-ex) ;
	
		// pos
		vpos( (v11*6+v14)/7, (v12*6+v15)/7, (v13*6+v16)/7 );
	
		// size of arrow
		r = dist(v11-v14, v12-v15, v13-v16) / dz / 25 ;
		bx = cos(a) * r ;
		by = sin(a) * r ;
	
		// draw
		_hsp.line( ex, ey,  dx - by, dy + bx);
		_hsp.line( ex, ey,  dx + by, dy - bx);
	}

	return;
}*/


// box �{�b�N�X��`�� (x1, y1, z1,  x2, y2, z2)
//-----------------------------------------------------------
void d3module::box( double v11, double v12, double v13,  double v14, double v15, double v16){

	line( v11, v12, v13,  v11, v12, v16) ;
	line( v11, v12, v16,  v11, v15, v16) ;
	line( v11, v15, v16,  v11, v15, v13) ;
	line( v11, v15, v13,  v11, v12, v13) ;

	line( v14, v12, v13,  v14, v15, v13) ;
	line( v14, v15, v13,  v14, v15, v16) ;
	line( v14, v15, v16,  v14, v12, v16) ;
	line( v14, v12, v16,  v14, v12, v13) ;

	line( v11, v12, v13,  v14, v12, v13) ;
	line( v11, v12, v16,  v14, v12, v16) ;
	line( v11, v15, v16,  v14, v15, v16) ;
	line( v11, v15, v13,  v14, v15, v13) ;

}
/*
// d3circle �~��`�� (x, y, z,  r,  flg)
//-----------------------------------------------------------
void d3module::circle( double ppx, double ppy, double ppz, double ppr, int ppf ){

	vpos( ppx, ppy, ppz );

	if (df) {
		r = ppr / dz ;
		if (r >= 1) {
//			_hsp.circle( dx-r, dy-r, dx+(r+0.5), dy+(r+0.5), ppf);
			_hsp.circle( dx-r, dy-r, dx+(r+0.5), dy+(r+0.5));
		} else {
			_hsp.pset( dx, dy);
		}
	}
	return;
	
}
	
// d3mes �������\�� (str,  x, y, z)
//-----------------------------------------------------------
void d3module::mes( char *pmsg, double ppx, double ppy, double ppz ) {

	vpos( ppx, ppy, ppz) ;

	if df {
		_hsp.pos( 0, -32000 );
		_hsp.mes( pmsg );
		_hsp.pos( dx - ginfo_mesx/2, dy - ginfo_mesy/2) ;
		_hsp.mes( pmsg);
	}
	return ;
}
*/

