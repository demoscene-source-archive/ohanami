#version 130
uniform sampler2D tex0;

//
// Ohanami
//
// 17:42 < ferris> "Ohana means flowers. Mi means see. And it’s means drinking alcohol party under the Sakura trees"
//
// 17:47 < ferris> fizzer: last thing, I mentioned if you use the title he'd probably get a thanks in the readme or something, and he requested to be called Can you write “Takeshi from TOCHKA”
//
//  お花見
//

float time=gl_TexCoord[0].x-11;
float seed;
float rand() { return fract(sin(seed++)*43758.545); }

vec2 t=(gl_FragCoord.xy-vec2(640,360))/640.;

float ti;
float col=1e3,col2=1e3,col3=1e3;
float f(vec3 p)
{
   col3=p.y-(.5+.5*cos(p.x*2))*.1;
   float d=max(col3,length(p.xz)-5.5);
   float s=1,ss=1.6;
if(ti<1)
 return d;  
   vec3 w=normalize(vec3(-1,1.2,-1));
   vec3 u=normalize(cross(w,vec3(0,1,0)));

if(ti>12&&ti<18)
{
col2=(length(mod(p*2-1.02+ti/2,2)-1)-mix(.001,.06,(ti-12)/5))/2;
d=min(d,col2);
}

   int j=int(min(floor(ti-1),7));

float scale=min(.3+ti/6,1);
p/=scale;

   for(int i=0;
       d=min(d,scale*max(p.y-1,max(-p.y,length(p.xz)-.1/(p.y+.7)))/s),
         p.xz=abs(p.xz),
         p.y-=1,
         i<j;
         p*=mat3(u,normalize(cross(u,w)),w),
         p*=ss,
         s*=ss,
       ++i);

if(ti<25)
   return d;
 
   return min(d,col=max(0,length(p)-.25)/s);
}

float hash(float n)
{
    return fract(sin(n)*43758.5453);
}

float noise(vec2 p)
{
   return hash(p.x + p.y*57.0);
}

float valnoise(vec2 p)
{
   vec2 c=floor(p);
   vec2 f=smoothstep(0.,1,fract(p));
   return mix (mix(noise(c+vec2(0,0)), noise(c+vec2(1,0)), f.x),
               mix(noise(c+vec2(0,1)), noise(c+vec2(1,1)), f.x), f.y);
}

vec4 samp(vec2 p)
{
   p.y*=1280./720.;
return texture(tex0,p/2+.5)*1.05;
}

void main()
{
if(gl_TexCoord[0].w>.5)
{

ti=max(0,time)/3.;
ti=noise(floor(gl_FragCoord.xy))+(floor(ti)+clamp(fract(ti)*2,0,1));
ti=floor(ti);

   float zoom=1.5;
   vec2 filmoffset=vec2(0);

   if(ti>8&&ti<12){zoom=ti-6;filmoffset=vec2(cos(ti*2),sin(ti*3))/3;}
//   if(ti>13&&ti<15){zoom=5;filmoffset=vec2(.8,.3);}
   if(ti>25&&ti<29){zoom=5.5;filmoffset=vec2(cos(ti*2),sin(ti*3))/3;}

   vec3 ro=vec3(-2.5,.5+cos(ti*17)*.1,3.);
   vec3 rd=normalize(vec3(t.xy+filmoffset,zoom));
if(ti==10)ro.y+=2;
   vec3 camtarget=vec3(0,1.3,0);
   
   vec3 w=normalize(camtarget-ro);
   vec3 u=normalize(cross(w,vec3(0,1,0)));
   vec3 v=normalize(cross(w,-u));

   rd=mat3(u,v,w)*rd;

      gl_FragColor.rgb=vec3(.8,.8,1)/6;
         float s=20.;

         float t=0.,d=0.;
         for(int i=0;i<100;++i)
         {
            d=f(ro+rd*t);
      if(d<1e-3)break;
            t+=d;
      if(t>10)return;
         }

      {
      vec3 rp=ro+rd*t;
      gl_FragColor.rgb=vec3(.75,.6,.4)/1.5;
      if(col<2e-3)gl_FragColor.rgb=vec3(1,.7,.8);
     if(col3<2e-2&&(ti<17||ti>22))gl_FragColor.rgb=vec3(.5,1,.6)/3;
      }

      vec3 ld=normalize(vec3(1,3+cos(ti)/2,1+sin(ti*3)/2));
      float e=1e-2;
         float d2=f(ro+rd*t+ld*e);
         float l=max(0,(d2-d)/e);

         float d3=f(ro+rd*t+vec3(0,1,0)*e);
         float l2=max(0,.5+.5*(d3-d)/e);

{
      vec3 rp=ro+rd*t;
if(ti>12&&ti<22)
if(col2<1e-2||d3+d2/7>0.0017&&pow(valnoise(rp.xz*8),2)>abs(ti-18)/5.)gl_FragColor.rgb=vec3(.65);
}


      vec3 ld=normalize(vec3(1,3+cos(ti)/2,1+sin(ti*3)/2));
      float e=1e-2;
         float d2=f(ro+rd*t+ld*e);
         float l=max(0,(d2-d)/e);

         float d3=f(ro+rd*t+vec3(0,1,0)*e);
         float l2=max(0,.5+.5*(d3-d)/e);

      {
      vec3 rp=ro+rd*t;
if(ti>12&&ti<17)
if(col2<1e-2||d3+d2/7>0.0017&&valnoise(rp.xz*8)<(ti-12)/3.)gl_FragColor.rgb=vec3(.65);
      }

         vec3 rp=ro+rd*(t-1e-3);

      t=0.1;
      float sh=1.;
      for(int i=0;i<30;++i)
      {
            d=f(rp+ld*t)+.01;
      sh=min(sh,d*50+0.3);
      if(d<1e-4)break;
            t+=d;
      }

      gl_FragColor.rgb*=1*sh*(.2+.8*l)*vec3(1,1,.9)*.7+l2*vec3(.85,.85,1)*.4;

}
else
{
   
   //if(t.x>0.)discard;
 //     if(abs(t.y)>.5){gl_FragColor=vec4(0);return;}
vec3 c=vec3(.8,.8,1)/6;
vec2 p=t.xy;
      for(int n=0;n<2;++n)
      {
      float maxr=mix(1./22.,1./4.,1.-n)*.4;

      ivec2 uo=ivec2(floor(p/maxr));
      for(int i=-1;i<2;++i)
      for(int j0=-1;j0<2;++j0)
      {
            vec2 u=uo+vec2(i,((uo.x+i)&1)==1?-j0:j0);
            seed=u.x*881+u.y*927+n*1801;
            for(int k=0;k<11;++k)
            {
               vec2 o=(u+vec2(rand(),rand()))*maxr;
               vec2 p2=p-o;
               vec3 cc=samp(o).rgb;
               float a=dot(cc,vec3(1./3.));
               float r=mix(.25,.99,pow(rand(),4))*maxr;
               float ang=rand()*acos(-1.)*2;
               float d=length(p2);
               p2*=mat2(cos(ang),sin(ang),-sin(ang),cos(ang));
               cc=mix(cc,vec3(a)*1.5,pow(rand(),16));
if(rand()>-floor(time*2)/2/10)
{
               c=mix(c,cc,mix(.1,.4,rand())*3*pow(a,.8)*mix(.8,1,cos(p2.x*1200)*.5+.5)*clamp((r-d)/mix(.001,.004,rand()),0,1));
               c=mix(c,cc/2.,mix(.14,.3,pow(rand(),16))/4*clamp(1-abs(r-d)/.002,0,1));
}
            }
      }

      }

vec2 e=vec2(1e-3,0);
vec2 p2=p+(valnoise(p*18)-.5)*.01;
float c0=dot(vec3(1./3.),samp(p2).rgb);
float c1=dot(vec3(1./3.),samp(p2+e.xy).rgb);
float c2=dot(vec3(1./3.),samp(p2+e.yx*1.8).rgb);

c*=vec3(mix(.1,1,1.-clamp(max(abs(c2-c0),abs(c1-c0))*4,0,.13)));
//c-=.3*vec3(1-mix(.1,1,1.-clamp(max(abs(c2-c0),abs(c1-c0))*4,0,.1)));


c=(c-.5)*1.1+.5;
c*=1.-smoothstep(60+42-11,60+48-11,time);
gl_FragColor.rgb=sqrt(c*mix(.9,1,valnoise(t.xy*400)))*1.28;

}

}

