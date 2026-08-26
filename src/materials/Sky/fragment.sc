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

  // texture-based volumetric aurora (custom NoiseVoxel texture)
  SAMPLER2D_AUTOREG(s_NoiseVoxel);

  float aurPow2(float x) { return x * x; }

  vec3 GetAurora(vec3 vDir, float time, float dither) {
    float VdotU = clamp(vDir.y, 0.0, 1.0);
    // smooth elevation band: feathered fade toward the horizon so there is
    // no hard circle edge - aurora just dissolves into the distance
    float band = smoothstep(0.05, 0.55, VdotU);
    float visibility = band * (4.0 - VdotU * 0.9);
    if (visibility <= 0.001) return vec3_splat(0.0);

    vec3 aurora = vec3_splat(0.0);
    vec3 wpos = vDir;
    // softer horizon clamp reduces the pinched stretch near the edge
    wpos.xz /= max(wpos.y, 0.18);

    // lighter than the reference 7-sample loop, same layered look
    const int sampleCount = 5;
    const int sampleCountP = sampleCount + 10;

    float ditherM = dither + 10.0;

    for (int i = 0; i < sampleCount; i++) {
      float current = aurPow2((float(i) + ditherM) / float(sampleCountP));
      vec2 planePos = wpos.xz * (0.8 + current) * 7.0;
      planePos *= 0.0007;
      float noise = texture2D(s_NoiseVoxel, planePos).r;
      noise = aurPow2(aurPow2(aurPow2(aurPow2(1.0 - 0.8 * abs(noise - 0.5)))));
      noise *= texture2D(s_NoiseVoxel, planePos * 8.0).b;
      noise *= texture2D(s_NoiseVoxel, planePos * 1.0).g;
      float currentM = 1.0 - current;
      aurora += noise * currentM * mix(NL_AURORA_TEX_COL1, NL_AURORA_TEX_COL2, aurPow2(aurPow2(currentM)));
    }

    aurora *= 3.8;
    return aurora * visibility / float(sampleCount);
  }

  // procedural dome clouds live in newb/functions/clouds.h
  // (nlVibrantClouds / nlVibrantCloudColor) so the water reflection in
  // RenderChunk can draw the exact same shapes.
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

    // texture-based aurora (night only, fades out during day)
    #ifdef NL_AURORA_TEX
      if (!env.underwater) {
        float nightFactor = 1.0 - smoothstep(0.0, 0.4, env.dayFactor);
        if (nightFactor > 0.001) {
          float dither = fract(sin(dot(viewDir.xy, vec2(12.9898, 78.233))) * 43758.5453);
          vec3 aurora = GetAurora(viewDir, v_underwaterRainTimeDay.z, dither);
          // rain hides the aurora behind the overcast sky
          skyColor += NL_AURORA_TEX*aurora*nightFactor*(1.0 - 0.85*env.rainFactor);
        }
      }
    #endif

    // procedural dome clouds (cheap, no texture)
    #ifdef NL_SKY_CLOUDS
      if (!env.underwater && viewDir.y > 0.001) {
        float scale = 0.8 / viewDir.y;
        float cloudA = nlVibrantClouds(viewDir.xz*scale, 0.004*scale, v_underwaterRainTimeDay.z);
        cloudA *= smoothstep(0.05, 0.35, viewDir.y);   // horizon fade
        cloudA *= NL_SKY_CLOUD_OPACITY;

        // cloud color tinted by sky/sun, darker at night
        vec3 cloudCol = nlVibrantCloudColor(env.dayFactor, sunLightTint(env.dayFactor, env.rainFactor));
        skyColor.rgb = mix(skyColor.rgb, cloudCol, clamp(cloudA, 0.0, 1.0));
      }
    #endif

    skyColor = colorCorrection(skyColor);

    gl_FragColor = vec4(skyColor, 1.0);
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
