//
// FILE: gfuncmin.cc -- Function minimization routines
//
// $Id$
//


#include <math.h>
#include "base/gstatus.h"
#include "math/gmath.h"
#include "math/gvector.h"
#include "gfunc.h"

static const double GOLD = 1.618034;
static const double TINY = 1.0e-20;

bool MinBracket(double tmin, double tmax,
		gFunction<double> &func,
		const gVector<double> &origin,
		const gVector<double> &direction,
		double &a, double &b, double &c)
{
  double tlim = tmax;
  double alpha = 0.5, beta = 0.5;
  int direc = 1;

  a = (tmin < 0.0) ? 0.0 : (3.0 * tmin + tmax) / 4.0;
  b = a + ((tmax - a > 4.0) ? 1.0 : (tmax - a) / 4.0);

  if (tmin >= a || a >= b || b >= tmax) {
    throw gFuncMinError();
  }

  gVector<double> scratch(direction);
  scratch *= a;
  scratch += origin;

  double fa = func.Value(scratch);

  scratch = direction;
  scratch *= b;
  scratch += origin;

  double fb = func.Value(scratch);

  if (fb > fa) {
    double dum = a;  a = b;  b = dum;
    dum = fb;  fb = fa;  fa = dum;
    direc = -1;  tlim = tmin;
  }

  c = b + GOLD * (b - a);

  if ((double) direc * (c - tlim) >= 0.0)
    c = alpha * b + beta * tlim;

  scratch = direction;
  scratch *= c;
  scratch += origin;

  double fc = func.Value(scratch);

  while (fb > fc) {
    double r = (b - a) * (fb - fc);
    double q = (b - c) * (fb - fa);
    double u = q - r;
    double fu;

    u = (u >= 0.0) ? gmax(u, TINY) : gmin(u, -TINY);
    u = b - ((b - c)* q - (b - a) * r) / (2.0 * u);
    
    double ulim = alpha * c + beta * tlim;

    if (abs(ulim - tlim) < TINY) {
      a = b;  b = c;  c = tlim;
      return false;
    }

    if ((b - u) * (u - c) > 0.0) {
      scratch = direction;
      scratch *= u;
      scratch += origin;

      fu = func.Value(scratch);
      if (fu < fc) {
	a = b; b = u;
	return true;
      }
      else if (fu > fb) {
	c = u;
	return true;
      }
      u = c + GOLD * (c - b);
    }
    else if ((c - u) * (u - ulim) > 0.0) {
      scratch = direction;
      scratch *= u;
      scratch += origin;

      fu = func.Value(scratch);

      if (fu < fc) {
	b = c;  c = u;
	fb = fc; fc = fu;
	u = c + GOLD * (c - b);
      }
    }
    else if ((u - ulim) * (ulim - c) >= 0.0)
      u = ulim;
    else 
      u = c + GOLD * (c - b);

    if ((double) direc * (u - ulim) > 0.0)  u = ulim;

    if ((-ulim - u) * (u - ulim) < 0.0)
      throw gFuncMinError();

    if (u == ulim) {
      alpha *= alpha;
      beta = 1.0 - alpha;
    }

    scratch = direction;
    scratch *= u;
    scratch += origin;

    fu = func.Value(scratch);
    a = b;  b = c;  c = u;
    fa = fb; fb = fc; fc = fu;
  }
  return true;
}


static const double CGOLD = 0.3819660;


double Brent(double ax, double bx, double cx,
	     gFunction<double> &func,
	     const gVector<double> &origin,
	     const gVector<double> &direction,
	     double &xmin,
	     int maxits, double tol)
{
  static const double EPSBRENT = 1.0e-10;
  static const double SMALLNUM = 1.0e-200;

  gVector<double> scratch(direction);

  double etemp,fu,u;
  double e = 0.0, d = 0.0;
  double a = gmin(ax,cx);
  double b = gmax(ax,cx);
  double x = bx, w = bx, v =bx;

  scratch *= x;
  scratch += origin;

  double fx = func.Value(scratch);

  double fw = fx, fv = fx;

  for (int iter = 1; iter <= maxits; iter++)  {
    double xm = 0.5 * (a + b);
    double tol1 = tol * abs(x) + EPSBRENT;
    double tol2 = 2.0 * tol1;

    if (abs(x - xm) <= (tol2 - 0.5 * (b - a)))  {
      //      double lhs, rhs;
      //      lhs = abs(x - xm);
      //      rhs = tol2 - 0.5 * (b-a);
      xmin = x;
      return fx;
    }

    if (abs(e) > tol1)	{
      double r = (x - w) * (fx - fv);
      double q = (x - v) * (fx - fw);
      double p = (x - v) * q - (x - w) * r;
      q = 2.0 * (q - r);
      if (q > 0.0)  p = -p;
      q = abs(q);
      etemp = e;
      e = d;

      if (abs(p) >= abs(0.5 * q * etemp) ||
	  p <= q * (a - x) || p >= q * (b - x)) {
	e = ((x >= xm) ? a : b) - x;
	d = CGOLD * e;
      }
      else {
	if (q <= SMALLNUM && q >= -SMALLNUM)  {
#ifdef DEBUG_WITH_GOUT
	  gout << "\nq=0";
#endif  // DEBUG_WITH_GOUT
	  return 0.0;
	}

	d = p / q;
	u = x + d;
	if (u - a < tol2 || b - u < tol2)  {
	  d = abs(tol1);
	  if (xm <= x)  d = -d;
	}
      }
    }
    else {
      e = ((x >= xm) ? a : b) - x;
      d = CGOLD * e;
    }

    if (abs(d) >= tol1)  
      u = x + d;
    else {
      u = x + abs(tol1);
      if (d <= 0.0)
	u = x - abs(tol1);
    }

    scratch = direction;
    scratch *= u;
    scratch += origin;
    
    fu = func.Value(scratch);
    if (fu <= fx) {
      if (u >= x) a = x; else b = x;
      v = w;  w = x;  x = u;
      fv = fw;  fw = fx;  fx = fu;
    }
    else {
      if (u < x) a = u; else b = u;
      if (fu <= fw || w == x) {
	v = w;  w = u;  fv = fw;  fw = fu;
      }
      else if (fu <= fv || v == x || v == w) {
	v = u;  fv = fu;
      }
    }
  }
#ifdef DEBUG_WITH_GOUT
  gout << "\nToo many iterations in BRENT";
#endif  // DEBUG_WITH_GOUT
  xmin = x;
  return fx;

}


double LineMin(double tmin, double tmax,
	       gFunction<double> &func,
	       const gVector<double> &origin,
	       const gVector<double> &direction,
	       double &xmin,
	       int maxitsBrent, double tolBrent,
	       gOutput &tracefile, int tracelevel = 0)
{
  double a, b, c;
  
  if(tracelevel > 0) {
    tracefile.SetExpMode() << "\nSearching from " << origin;
    tracefile << "\n    Direction: " << direction;
    tracefile.SetFloatMode();
  }

  if (tmin >= tmax)
    throw gFuncMinError();

  if (!MinBracket(tmin, tmax, func, origin, direction, a, b, c))  {
    xmin = c;

    if(tracelevel > 0) {
      tracefile << "\n   Bracketing failed.";
      tracefile.SetExpMode() << "\n   step size = " << xmin;
      tracefile.SetFloatMode();
    }
    return func.Value(origin + direction * c);
  }

  double fret = Brent(a, b, c, func, origin, direction, xmin, maxitsBrent, tolBrent);

  
  if(tracelevel > 0) {
    tracefile.SetExpMode() << "\n   step size = " << xmin << "  value = " << fret;
    tracefile.SetFloatMode();
  }
  return fret;
}



void RayMin(gFunction<double> &func,
	    gVector<double> &v, gVector<double> &xi,
	    double &fret,
	    int maxitsBrent, double tolBrent,
	    gOutput &tracefile,int tracelevel = 0,bool interior = false)
{
  static const double BIGNUM = 1.0e20;
  static const double SMALLNUM = 1.0e-200;

  double tjmin = -BIGNUM, tjmax = BIGNUM;
  double xmin;

// constraints to keep in simplex
// this is sort of in the "wrong place"... but I'm not sure how to
// deal with it properly yet, so I'll leave well enough alone for now 
// removing this causes argument domain errors in log() calls in
// some algorithms.  

  for (int j = 1; j <= xi.Length(); j++)
    if (xi[j] >= SMALLNUM || xi[j] <= -SMALLNUM)  {
      double tt = -v[j] / xi[j];
      if (tt < 0.0 && tt > tjmin)  tjmin = tt;
      if (tt > 0.0 && tt < tjmax)  tjmax = tt;
    }
  double delta = 0.99; 
  if(interior) {tjmin=delta*tjmin;tjmax=delta*tjmax;}
  fret = LineMin(tjmin, tjmax, func, v, xi, xmin, maxitsBrent, tolBrent,
	         tracefile,tracelevel);

  xi *= xmin;
  v += xi;
  if (interior)
    for (int kk = 1; kk <= v.Length(); kk++) {
      if (v[kk] <= 0.0)
	throw gFuncMinError();
    }
}


#include "math/gpvector.h"

void Project(gVector<double> &x,
	     const gArray<int> &lengths)
{
  int index = 1;
  for (int part = 1; part <= lengths.Length(); part++)  {
    double avg = 0.0;
    int j;
    for (j = 1; j <= lengths[part]; j++, index++)  {
      avg += x[index];
    }
    avg /= (double) lengths[part];
    index -= lengths[part];
    for (j = 1; j <= lengths[part]; j++, index++)  {
      x[index] -= avg;
    }
  }
}

#include "math/gmatrix.h"

bool DFP(gPVector<double> &p,
	 gC2Function<double> &func,
	 double &fret, int &iter,
         int maxits1, double tol1, int maxitsN, double tolN,
	 gOutput &tracefile, int tracelevel, bool interior,
	 gStatus &status)
{
  int i, j, its;
  int n = p.Length();
  double fp, fae, fad, fac;
  gMatrix<double> hessin(n, n);
  gVector<double> xi(n), g(n), dg(n), hdg(n);

  fp = func.Value(p);
  double startVal = fp;
  func.Deriv(p, g);
  xi = -g;
  
  fret = func.Value(p);
  if (tracelevel > 0)  {
    tracefile.SetExpMode() << "\n\nDFP start:  val = " << fret << " ";
    tracefile.SetFloatMode();
    tracefile << " p = " << p;
  }

  for (i = 1; i <= n; i++) {
    for (j = 1; j <= n; j++)  hessin(i, j) = 0.0;
    hessin(i, i) = 1.0;
  }
  
  for (its = 1; its <= maxitsN; its++)  {
    status.Get();
    iter = its;
    Project(xi, p.Lengths());
    RayMin(func, p, xi, fret, maxits1, tol1, tracefile,tracelevel-1,interior);
    
    if (fret <= tolN || fret >= fp || its >= maxitsN)  {
      if (fret <= tolN)  return true;
      else               return false;
    }

    fp = fret;
    if(startVal>tolN)
      status.SetProgress((double)(startVal-fp)/(double)(startVal-tolN));
    dg = g;
    fret = func.Value(p);
    func.Deriv(p, g);
    Project(g, p.Lengths());
    dg = g - dg;
    hdg = hessin * dg;

    fac = dg * xi;
    fae = dg * hdg;
    
    if (fac == 0.0) 
      fac = TINY;
    
    if (fae == 0.0)
      fae = TINY;

    fac = 1.0 / fac;
    fad = 1.0 / fae;

    dg = (xi * fac) - (hdg * fad);

    for (i = 1; i <= n; i++)
      for (j = 1; j <= n; j++)
	hessin(i, j) += (fac * (xi[i] * xi[j])) - (fad * (hdg[i] * hdg[j])) +
                 	(fae * (dg[i] * dg[j]));
    xi = -(hessin * g);
    
    if (tracelevel > 0) {
      tracefile << "\nDFP iter: " << iter;
      tracefile.SetExpMode();
      tracefile << " val = " << fret;
      tracefile.SetFloatMode();
      tracefile << " p = " << p;
    }
    if (tracelevel > 1 )   {
      tracefile << "\nHessian:\n";
      tracefile << xi;
    }

  }

#ifdef DEBUG_WITH_GOUT
  gout << "Too many iterations in DFP";
#endif  // DEBUG_WITH_GOUT
  return false;
}


//
// New version of Powell that keeps on the hyperplane of the simplices
//
bool Powell(gPVector<double> &p,
	    gMatrix<double> &xi,
	    gFunction<double> &func,
	    double &fret, int &iter,
	    int maxits1, double tol1, int maxitsN, double tolN,
	    gOutput &tracefile, int tracelevel, bool interior,
	    gStatus &status)
{
  int ibig,n;
  double t,fptt,fp,del;
  double startVal;
  
  n=p.Length();
  gVector<double> pt(n);
  gVector<double> ptt(n);
  gVector<double> xit(n);
  fret=func.Value(p);
  startVal = fret;
  
  if (tracelevel > 0)  {
    tracefile.SetExpMode() << "\n\nPow start:  val = " << fret << " ";
    tracefile.SetFloatMode();
    tracefile << " p = " << p;
  }

  pt=p;
  for (iter=1; ;iter++) {
    status.Get();
    fp=fret;
    if(startVal>tolN)
      status.SetProgress((double)(startVal-fp)/(double)(startVal-tolN));

    ibig=0;
    del=0.0;
    int index = 1;
    for (int part = 1; part <= p.Lengths().Length(); part++, index++)  {
      for (int j = 1; j < p.Lengths()[part]; j++, index++)   {
	xi.GetRow(index, xit);
	fptt=fret;

	RayMin(func, p, xit, fret, maxits1, tol1, tracefile,tracelevel-1,interior);
      
	if (fptt-fret > del) {
	  del=fptt-fret;
	  ibig=index;
	}
      }
    }
    
    if (tracelevel > 0) {
      tracefile << "\nPow iter: " << iter;
      tracefile.SetExpMode() << " val = " << fret << " ";
      tracefile.SetFloatMode();
      tracefile << " p = " << p;
    }

    status.Get();
    if (fret <= tolN) return true;

    if (iter == maxitsN)   {
      if (tracelevel > 0)  {
	tracefile << "\nlocation = " << p;
	tracefile.SetExpMode() << " value = " << fret << "\n\n";
	tracefile.SetFloatMode() << "Powell failed to converge in " << iter << " iterations\n\n";
      }
      return false;
    }
    
    ptt=((gVector<double> &) p)*2.0-pt;
    xit=((gVector<double> &) p)-pt;

    pt=p;
    
    fptt=func.Value(ptt);
    if (fptt < fp) {
      t=2.0*(fp-2.0*fret+fptt)*pow(fp-fret-del,(long)2)-del*pow(fp-fptt,(long)2);
      if (t < 0.0) {
	Project(xit, p.Lengths());
	RayMin(func, p, xit, fret, maxits1, tol1, tracefile, tracelevel-2,interior);
	xi.SetRow(ibig, xit);
	if (interior)
	  for (int kk = 1; kk < p.Length(); kk++) {
	    if (p[kk] <= 0.0)
	      throw gFuncMinError();
	  }
      }
    }
    
    if (tracelevel > 1)   {
      tracefile.SetExpMode() << "\nApproximate Hessian:\n";
      tracefile << xi;

      tracefile << "\nlocation = " << p << " value = " << fret;
      tracefile.SetFloatMode();
    }
  }
}


bool OldPowell(gVector<double> &p,
	    gMatrix<double> &xi,
	    gFunction<double> &func,
	    double &fret, int &iter,
	    int maxits1, double tol1, int maxitsN, double tolN,
	    gOutput &tracefile, int tracelevel, gStatus &status)
{
  int i,ibig,n;
  double t,fptt,fp,del;
  double startVal;
  
  n=p.Length();
  gVector<double> pt(1,n);
  gVector<double> ptt(1,n);
  gVector<double> xit(1,n);
  fret=func.Value(p);
  startVal = fret;

  if (tracelevel > 0)  {
    tracefile.SetExpMode() << "\nPow start:  val = " << fret << " ";
    tracefile.SetFloatMode();
    tracefile << " p = " << p;
  }
  
  pt=p;
  for (iter=1;;iter++) {
    fp=fret;
    if(startVal>tolN)
      status.SetProgress((double)(startVal-fp)/(double)(startVal-tolN));
    ibig=0;
    del=0.0;
    for (i=1;i<=n;i++) {
      xi.GetRow(i, xit);
      fptt=fret;
      
      RayMin(func, p, xit, fret, maxits1, tol1, tracefile, tracelevel-1,true);
//      RayMin(func, p, xit, fret, maxits1, tol1, tracefile);
      
      if (fptt-fret > del) {
	del=fptt-fret;
	ibig=i;
      }
    }
    
    if (tracelevel > 0) {
      tracefile << "\nPow iter: " << iter;
      int prec = tracefile.GetPrec();
      tracefile.SetExpMode().SetPrec(15) << " val = " << fret << " ";
      tracefile << " p = " << p;
      tracefile.SetFloatMode().SetPrec(prec);
    }
    status.Get();
    if (fret <= tolN) return true;

    if (iter == maxitsN) {
      if (tracelevel > 0)  {
	tracefile << "\nlocation = " << p;
	tracefile.SetExpMode() << " value = " << fret;
	tracefile.SetFloatMode() << "\n\nPowell failed to converge in " << iter << " iterations\n";
      }
      return false;
    }
    
    ptt=p*2.0-pt;
    xit=p-pt;
    
    pt=p;
    
    fptt=func.Value(ptt);
    if (fptt < fp) {
      t=2.0*(fp-2.0*fret+fptt)*pow(fp-fret-del,(long)2)-del*pow(fp-fptt,(long)2);
      if (t < 0.0) {
	RayMin(func, p, xit, fret, maxits1, tol1, tracefile, tracelevel-2,true);
//	RayMin(func, p, xit, fret, maxits1, tol1, tracefile);
	xi.SetRow(ibig, xit);
      }
    }

    if (tracelevel > 1)   {
      tracefile.SetExpMode() << "\nApproximate Hessian:\n";
      tracefile << xi;
      
      tracefile << "\nlocation = " << p << " value = " << fret;
      tracefile.SetFloatMode();
    }
  }
}



