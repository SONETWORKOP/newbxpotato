#ifndef CLOUDS_H
#define CLOUDS_H

#include "detection.h"
#include "noise.h"
#include "sky.h"

// simple clouds 2D noise
float cloudNoise2D(vec2 p, highp float t, float rain) {
  t *= NL_CLOUD1_SPEED;
  p += t;
  p.y += 3.0*sin(0.3*p.x + 0.1*t);

  vec2 p0 = floor(p);
  vec2 u = p-p0;
  u *= u*(3.0-2.0*u);
  vec2 v = 1.0-u;

  // multi-octave noise for more natural cloud shapes
  float n = mix(
    mix(rand(p0),rand(p0+vec2(1.0,0.0)), u.x),
    mix(rand(p0+vec2(0.0,1.0)),rand(p0+vec2(1.0,1.0)), u.x),
    u.y
  );
  n *= 0.5 + 0.5*sin(p.x*0.6 - 0.5*t)*sin(p.y*0.6 + 0.8*t);

  // second octave for detail
  vec2 p1 = p*2.5 + vec2(1.7, 3.2);
  vec2 p1f = floor(p1);
  vec2 u1 = p1-p1f;
  u1 *= u1*(3.0-2.0*u1);
  float n2 = mix(
    mix(rand(p1f),rand(p1f+vec2(1.0,0.0)), u1.x),
    mix(rand(p1f+vec2(0.0,1.0)),rand(p1f+vec2(1.0,1.0)), u1.x),
    u1.y
  );
  n = mix(n, n2, 0.35);

  n = min(n*(1.0+rain), 1.0);
  return n*n;
}

/* ---- Pixel clouds ----
   Direct implementation of the reference "pixelated" cloud shader:

     hash(p)      = fract(cos(p.x + p.y*332.0) * 335.552)
     pixelated(uv): uv *= 5.0; 10 iterations of { uv /= 1.007;
                    a = mix(a, 1.0, step(0.7, hash(floor(uv + t)))) }
                    then subtract 0.3 * the last cell's step value

   The lattice is sampled with floor(), so the clouds keep their blocky
   pixel-art silhouette. Each iteration shrinks the lattice slightly, which
   stacks 10 offset copies of the same cell grid into layered blobs.
   NL_PIXEL_CLOUD_STEPS trades iterations for GPU time (10 = reference).
*/
float nlPixelCloudHash(highp vec2 p) {
  return fract(cos(p.x + p.y*332.0)*335.552);
}

// returns .x = cloud coverage, .y = top-layer mask (used for shading)
vec2 nlPixelCloudLayers(vec2 uv, highp float t) {
  float drift = -t*NL_PIXEL_CLOUD_SPEED;

  uv *= NL_PIXEL_CLOUD_SCALE;

  float a = 0.0;
  for (int i = 0; i < NL_PIXEL_CLOUD_STEPS; i++) {
    uv /= 1.007;
    float c = step(NL_PIXEL_CLOUD_COVERAGE, nlPixelCloudHash(floor(uv + drift)));
    a = mix(a, 1.0, c);
  }

  // the reference darkens the final layer against the stack, which is what
  // gives the clouds their internal shading instead of a flat white mass
  float b = step(NL_PIXEL_CLOUD_COVERAGE, nlPixelCloudHash(floor(uv + drift)));

  return vec2(clamp(a - b*0.3, 0.0, 1.0), b);
}

// Cloud color. Sunlit tops stay bright, the shaded layer picks up the horizon
// tint so clouds sit in the sky instead of floating on top of it.
vec3 nlPixelCloudColor(float shade, float dayFactor, float rain, vec3 horizonCol) {
  vec3 lit = mix(NL_PIXEL_CLOUD_NIGHT_COL, NL_PIXEL_CLOUD_DAY_COL,
                 smoothstep(-0.1, 0.35, dayFactor));
  vec3 shadowCol = mix(lit*0.42, horizonCol, 0.45);
  vec3 col = mix(lit, shadowCol, shade*NL_PIXEL_CLOUD_SHADING);
  return col*(1.0 - 0.55*rain);
}

// viewDir must be normalized. Projects the view ray onto the cloud plane the
// same way the reference does (p.xz * 0.8 / p.y) and adds world offset so the
// clouds stay fixed in the world while the player moves.
vec4 renderPixelClouds(
    vec3 viewDir, vec2 cameraPos, highp float t, float rain, float dayFactor, vec3 horizonCol
) {
  float invY = 0.8/max(viewDir.y, 0.025);
  vec2 uv = viewDir.xz*invY + cameraPos*NL_PIXEL_CLOUD_WORLD_SCALE;

  vec2 layers = nlPixelCloudLayers(uv, t);

  // reference fade: smoothstep(0.2, 0.9, p.y) - keeps the horizon clear so the
  // pixel lattice never smears into an aliased band
  float alpha = layers.x*smoothstep(0.06, 0.42, viewDir.y);
  alpha *= NL_PIXEL_CLOUD_OPACITY*(1.0 - 0.25*rain);

  return vec4(nlPixelCloudColor(layers.y, dayFactor, rain, horizonCol), clamp(alpha, 0.0, 1.0));
}

// hash13 - 3D hash for the soft fBm cloud/reflection noise
float nlHash13(vec3 p) {
  p = fract(p * 0.1031);
  p += dot(p, p.yzx + 33.33);
  return fract((p.x + p.y) * p.z);
}

// Soft multi-octave value noise. Three octaves keep the reflection cheap.
float cloudFbm2D(vec2 p, highp float t) {
  // slow drift + gentle sway
  p += NL_CLOUD1_SPEED*t;
  p.y += 3.0*sin(0.35*p.x + 0.12*t);

  float amp = 1.0;
  float freq = 1.0;
  float n = 0.0;
  float total = 0.0;
  for (int i = 0; i < 3; i++) {
    vec2 q = p*freq;
    vec2 c = floor(q);
    vec2 f = q - c;
    f = f*f*(3.0-2.0*f);
    float v = mix(
      mix(nlHash13(vec3(c, 0.0)), nlHash13(vec3(c+vec2(1.0,0.0), 0.0)), f.x),
      mix(nlHash13(vec3(c+vec2(0.0,1.0), 0.0)), nlHash13(vec3(c+vec2(1.0,1.0), 0.0)), f.x),
      f.y
    );
    n += amp*v;
    total += amp;
    amp *= 0.52;
    freq *= 2.13;
  }
  return n/total;
}

// realistic cloud density - soft fBm with gentle coverage ramp
float cloudDensity2D(vec2 p, highp float t, float rain) {
  float n = cloudFbm2D(p, t);
  // wide soft ramp so clouds have fluffy rounded edges, not cube steps
  float cover = 0.44 - 0.18*rain;
  n = smoothstep(cover, cover+0.42, n);
  // curvature makes the cores fuller and the edges feathered
  n = n*n*(1.45 - 0.45*n);
  return n;
}

// Reflection clouds. Water and wet ground mirror this instead of the pixel
// lattice: a floor()-based lattice aliases badly once it is bent through a
// rippling normal, so the mirror uses the smooth fBm shape at matching scale.
vec4 renderCloudsSimple(nl_skycolor skycol, vec3 pos, highp float t, float rain, vec3 viewDir, vec3 sunDir, vec3 sunCol) {
  pos.xz *= NL_CLOUD1_SCALE;
  float d = cloudDensity2D(pos.xz, t, rain);
  d = smoothstep(0.10, 0.46, d);

  vec3 lit = mix(NL_PIXEL_CLOUD_NIGHT_COL, NL_PIXEL_CLOUD_DAY_COL,
                 smoothstep(-0.1, 0.35, sunDir.y));
  vec3 shadowCol = mix(lit*0.42, skycol.horizon, 0.45);
  vec3 cloudBase = mix(shadowCol, lit, smoothstep(0.0, 0.62, d));

  vec4 col = vec4(cloudBase, smoothstep(0.06, 0.5, d));

  // thick cores read brighter than thin wisps
  col.rgb += smoothstep(0.25, 0.75, d)*vec3(0.16, 0.15, 0.12);

  // forward scattering towards the sun/moon keeps the mirror consistent with
  // the sky brightness at low sun angles
  float mu = dot(viewDir, sunDir);
  col.rgb += pow(max(mu, 0.0), 5.0)*sunCol*0.7*col.a;

  col.rgb *= 1.0 - 0.55*rain;
  return col;
}

// rounded clouds

// rounded clouds 3D density map
float cloudDf(vec3 pos, float rain, vec2 boxiness) {
  boxiness *= 0.999;
  vec2 p0 = floor(pos.xz);
  vec2 u = max((pos.xz-p0-boxiness.x)/(1.0-boxiness.x), 0.0);
  u *= u*(3.0 - 2.0*u);

  vec4 r = vec4(rand(p0), rand(p0+vec2(1.0,0.0)), rand(p0+vec2(1.0,1.0)), rand(p0+vec2(0.0,1.0)));
  r = smoothstep(0.1001+0.2*rain, 0.1+0.2*rain*rain, r); // rain transition

  float n = mix(mix(r.x,r.y,u.x), mix(r.w,r.z,u.x), u.y);

  // round y
  n *= 1.0 - 1.5*smoothstep(boxiness.y, 2.0 - boxiness.y, 2.0*abs(pos.y-0.5));

  n = max(1.25*(n-0.2), 0.0); // smoothstep(0.2, 1.0, n)
  n *= n*(3.0 - 2.0*n);
  return n;
}

vec4 renderCloudsRounded(
    vec3 vDir, vec3 vPos, float rain, float time, vec3 horizonCol, vec3 zenithCol,
    const int steps, const float thickness, const float thickness_rain, const float speed,
    const vec2 scale, const float density, const vec2 boxiness
) {
  float height = 7.0*mix(thickness, thickness_rain, rain);
  float stepsf = float(steps);

  // scaled ray offset
  vec3 deltaP;
  deltaP.y = 1.0;
  deltaP.xz = height*scale*vDir.xz/(0.02+0.98*abs(vDir.y));

  // local cloud pos
  vec3 pos;
  pos.y = 0.0;
  pos.xz = scale*(vPos.xz + vec2(1.0,0.5)*(time*speed));
  pos += deltaP;

  deltaP /= -stepsf;

  // alpha, gradient
  vec2 d = vec2(0.0,1.0);
  for (int i=1; i<=steps; i++) {
    float m = cloudDf(pos, rain, boxiness);
    d.x += m;
    d.y = mix(d.y, pos.y, m);
    pos += deltaP;
  }
  d.x *= smoothstep(0.03, 0.1, d.x);
  d.x /= (stepsf/density) + d.x;

  if (vPos.y < 0.0) { // view from top
    d.y = 1.0 - d.y;
  }

  // realistic cloud colors - white/grey with depth
  vec3 cloudTop = vec3(0.92, 0.95, 1.0);
  vec3 cloudBottom = mix(vec3(0.5, 0.55, 0.65), horizonCol * 0.6, 0.3);
  vec4 col = vec4(mix(cloudBottom, cloudTop, d.y), d.x);
  col.rgb += dot(col.rgb, vec3(0.12,0.1,0.08))*d.y*d.y;
  col.rgb *= 1.0 - 0.75*rain;
  return col;
}

float cloudsNoiseVr(vec2 p, float t) {
  float n = fastVoronoi2(p + t, 1.8);
  n *= fastVoronoi2(3.0*p + t, 1.5);
  n *= fastVoronoi2(9.0*p + t, 0.4);
  n *= fastVoronoi2(27.0*p + t, 0.1);
  //n *= fastVoronoi2(82.0*pos + t, 0.02); // more quality
  return n*n;
}

vec4 renderClouds(vec2 p, float t, float rain, vec3 horizonCol, vec3 zenithCol, const vec2 scale, const float velocity, const float shadow) {
  p *= scale;
  t *= velocity;

  // layer 1
  float a = cloudsNoiseVr(p, t);
  float b = cloudsNoiseVr(p + NL_CLOUD3_SHADOW_OFFSET*scale, t);

  // layer 2
  p = 1.4 * p.yx + vec2(7.8, 9.2);
  t *= 0.5;
  float c = cloudsNoiseVr(p, t);
  float d = cloudsNoiseVr(p + NL_CLOUD3_SHADOW_OFFSET*scale, t);

  // higher = less clouds thickness
  // lower separation between x & y = sharper
  vec2 tr = vec2(0.6, 0.7) - 0.12*rain;
  a = smoothstep(tr.x, tr.y, a);
  c = smoothstep(tr.x, tr.y, c);

  // shadow
  b *= smoothstep(0.2, 0.8, b);
  d *= smoothstep(0.2, 0.8, d);

  vec4 col;
  col.a = a + c*(1.0-a);
  // realistic white/grey clouds with depth shadow
  vec3 cloudWhite = vec3(0.93, 0.95, 1.0);
  vec3 cloudShadow = mix(horizonCol * 0.5, vec3(0.5, 0.55, 0.65), 0.4);
  col.rgb = mix(cloudShadow, cloudWhite, shadow*mix(b, d, c));
  col.rgb *= 1.0-0.65*rain;

  return col;
}

// aurora is rendered on clouds layer
#ifdef NL_AURORA
vec4 renderAurora(vec3 p, float t, float rain, vec3 FOG_COLOR) {
  t *= NL_AURORA_VELOCITY;
  p.xz *= NL_AURORA_SCALE;
  p.xz += 0.05*sin(p.x*4.0 + 20.0*t);

  float d0 = sin(p.x*0.1 + t + sin(p.z*0.2));
  float d1 = sin(p.z*0.1 - t + sin(p.x*0.2));
  float d2 = sin(p.z*0.1 + 1.0*sin(d0 + d1*2.0) + d1*2.0 + d0*1.0);
  d0 *= d0; d1 *= d1; d2 *= d2;
  d2 = d0/(1.0 + d2/NL_AURORA_WIDTH);

  float mask = (1.0-0.8*rain)*max(1.0 - 4.0*max(FOG_COLOR.b, FOG_COLOR.g), 0.0);
  return vec4(NL_AURORA*mix(NL_AURORA_COL1,NL_AURORA_COL2,d1),1.0)*d2*mask;
}
#endif

vec4 nlCloudAuroraReflection(nl_skycolor skycol, nl_environment env, vec3 viewDir, vec3 wPos, vec3 CAMERA_POS, highp float t, float cloudAmount) {
  vec2 cloudPos = wPos.xz;
  float viewDirY = viewDir.y >= 0.0 ? max(viewDir.y, 0.01) : min(viewDir.y, -0.01);
  float surfaceY = wPos.y + CAMERA_POS.y;
  // renderCloudsSimple / renderAurora are plane samplers, so project the
  // reflected ray onto the real cloud layer. A tiny fixed depth collapses the
  // projection and kills all parallax.
  vec2 projectionOffset = (NL_WATER_CLOUD_HEIGHT-surfaceY)*viewDir.xz/viewDirY;
  cloudPos += clamp(projectionOffset, -vec2_splat(4096.0), vec2_splat(4096.0));
  float fade = clamp(2.0 - 0.005*length(cloudPos), 0.0, 1.0);
  cloudPos += CAMERA_POS.xz;

  vec4 refl = vec4_splat(0.0);

  #ifdef NL_AURORA
    vec4 aurora = renderAurora(cloudPos.xyy, t, env.rainFactor, env.fogCol);
    aurora.a *= fade;
    refl = vec4(2.0*aurora.rgb*aurora.a, aurora.a);
  #endif

  // Cloud reflection always uses the lightweight Simple cloud algorithm,
  // regardless of which cloud renderer is active for the main sky. This keeps
  // reflections cheap and working in all cases.
  // cloudAmount = 0.0 lets a caller take only the aurora layer (water uses its
  // own per-pixel cloud mirror so it must not draw a second cloud shape here).
  if (cloudAmount > 0.0) {
    vec3 mainSunDir = env.sunDir.y > 0.0 ? env.sunDir : env.moonDir;
    vec4 clouds = renderCloudsSimple(skycol, cloudPos.xyy, t, env.rainFactor, viewDir, mainSunDir, vec3(1.0,0.95,0.85));
    clouds.a *= fade*cloudAmount;
    refl = vec4(mix(refl.rgb, clouds.rgb, clouds.a), min(refl.a + clouds.a, 1.0));
  }

  return refl;
}

#endif
