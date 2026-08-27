#ifndef INSTANCING
  $input v_worldPos, v_underwaterRainTimeDay
#endif

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <newb/main.sh>
  uniform vec4 TimeOfDay;
  uniform vec4 Day;
  uniform vec4 FogColor;
  uniform vec4 FogAndDistanceControl;
  uniform vec4 CameraPosition;

  // noise texture used by the aurora curtain
  SAMPLER2D_AUTOREG(s_AuroraNoise);

  float aurPow2(float x) { return x*x; }
  float aurPow1_5(float x) { return x*sqrt(max(x, 0.0)); }
  float aurClamp01(float x) { return clamp(x, 0.0, 1.0); }
  float aurSqrt1(float x) { return sqrt(max(x, 0.0)); }

  /*
    Aurora borealis - implementation of the reference GetAuroraBorealis():

      visibility = sqrt(clamp01(VdotU*1.5 - 0.225)) - sunVisibility - rain
      visibility *= 1.0 - VdotU*0.9
      wpos.xz /= max(0.0001, wpos.y)
      for each layer:
        current  = pow2((i + dither) / (layers + 10))
        planePos = wpos.xz*(0.8 + current)*11.0 + cameraPos
        planePos = floor(planePos)*0.0007              (STYLE 1)
        n  = noise(planePos).b
        n  = pow2^4(1.0 - 2.0*abs(n - 0.5))
        n *= pow1_5(noise(planePos*100.0 + animate).b)
        aurora += n*(1-current)*mix(COL1, COL2, pow2(pow2(1-current)))
      aurora *= 1.3 / layers

    floor() on planePos is what gives the curtain its stepped, banded look.
    Layer count is exposed as NL_AURORA_LAYERS (reference used 20; fewer layers
    is the single biggest performance lever here).
  */
  vec3 GetAuroraBorealis(vec3 vDir, float VdotU, float dither, float rain, vec2 camPosXZ, highp float t) {
    float visibility = aurSqrt1(aurClamp01(VdotU*1.5 - 0.225)) - rain;
    visibility *= 1.0 - VdotU*0.9;
    if (visibility <= 0.0) return vec3_splat(0.0);

    vec3 aurora = vec3_splat(0.0);

    vec3 wpos = vDir;
    wpos.xz /= max(0.0001, wpos.y);
    vec2 cameraPositionM = camPosXZ*0.0075;
    cameraPositionM.x += t*0.04;

    const int sampleCountP = NL_AURORA_LAYERS + 10;
    float ditherM = dither + 5.0;
    float auroraAnimate = t*0.001;

    for (int i = 0; i < NL_AURORA_LAYERS; i++) {
      float current = aurPow2((float(i) + ditherM)/float(sampleCountP));

      vec2 planePos = wpos.xz*(0.8 + current)*11.0 + cameraPositionM;
      planePos = floor(planePos)*0.0007;

      float n = texture2D(s_AuroraNoise, planePos).b;
      n = aurPow2(aurPow2(aurPow2(aurPow2(1.0 - 2.0*abs(n - 0.5)))));
      n *= aurPow1_5(texture2D(s_AuroraNoise, planePos*100.0 + auroraAnimate).b);

      float currentM = 1.0 - current;
      aurora += n*currentM*mix(NL_AURORA_COL_LOW, NL_AURORA_COL_HIGH, aurPow2(aurPow2(currentM)));
    }

    aurora *= 1.3*NL_AURORA_BRIGHTNESS;
    return aurora*visibility/float(NL_AURORA_LAYERS);
  }
#endif

void main() {
  #ifndef INSTANCING
    vec3 viewDir = normalize(v_worldPos);

    nl_environment env;
    env.end = false;
    env.nether = false;
    env.underwater = v_underwaterRainTimeDay.x > 0.5;
    env.rainFactor = v_underwaterRainTimeDay.y;
    env.dayFactor = v_underwaterRainTimeDay.w;
    env.fogCol = FogColor.rgb;
    env = calculateSunParams(env, TimeOfDay.x, Day.x);

    nl_skycolor skycol = nlOverworldSkyColors(env);

    vec3 skyColor = nlRenderSky(skycol, env, -viewDir, v_underwaterRainTimeDay.z, true);
    #ifdef NL_SHOOTING_STAR
      skyColor += NL_SHOOTING_STAR*nlRenderShootingStar(viewDir, env.fogCol, v_underwaterRainTimeDay.z);
    #endif
    #ifdef NL_GALAXY_STARS
      skyColor += NL_GALAXY_STARS*nlRenderGalaxy(viewDir, env.fogCol, env, v_underwaterRainTimeDay.z);
    #endif

    // aurora borealis - night only, hidden by rain and underwater fog
    #ifdef NL_AURORA
      if (!env.underwater) {
        float VdotU = clamp(viewDir.y, 0.0, 1.0);
        // sunVisibility in the reference; here the sky already knows dayFactor
        float nightFactor = 1.0 - smoothstep(-0.02, 0.32, env.dayFactor);
        if (nightFactor > 0.001 && VdotU > 0.15) {
          float dither = fract(sin(dot(viewDir.xy, vec2(12.9898, 78.233)))*43758.5453);
          vec3 aurora = GetAuroraBorealis(
            viewDir, VdotU, dither, env.rainFactor,
            CameraPosition.xz, v_underwaterRainTimeDay.z
          );
          skyColor += aurora*nightFactor;
        }
      }
    #endif

    skyColor = colorCorrection(skyColor);

    gl_FragColor = vec4(skyColor, 1.0);
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
