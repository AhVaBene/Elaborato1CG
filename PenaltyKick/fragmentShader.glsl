uniform float time;
uniform vec2 resolution;
uniform int sceltaFS;
in vec4 ourColor;
vec3 color;


vec3 glow(vec2 p, vec2 lpos)
{
    vec2 q = p - lpos;
    float atten = 1./dot(q,q);
    atten *= (1. + atten*1e-4); // Make the inside slightly sharper
    return vec3(1.0) * atten;
}

vec3 lastExplosion(float time)
{
    // vec3(time since last explosion,
    //      index of last explosion,
    //      time until next explosion)
    float t = mod(time, 10.);
    float interval = floor(time/10.);
    float t0max = 0., imax=-1.;
    float t0next = 10.;
    for(float i=0.; i<10.; i++)
    {
        float t0 = 5.0;
        if(t > t0 && t0 > t0max)
        {
            t0max = t0;
            imax = i;
        }
        if(t < t0 && t0 < t0next)
        {
            t0next = t0;
        }
    }
    return vec3(t-t0max, 10.*interval+imax, t0next-t);
}

void main()
{
    if (sceltaFS==0)
    {
        gl_FragColor = vec4(ourColor);
    } else{
        vec2 p = vec2((gl_FragCoord.x / resolution.x- 0.5) * 2.0, (gl_FragCoord.y / resolution.y - 0.9) * 2.0);

        vec3 col = vec3(0);
    
        vec3 lastExpl = lastExplosion(time);
        float t = lastExpl.x, explNum = lastExpl.y, tFadeout = lastExpl.z;
    
        // Fireworks base color
        vec3 baseCol = vec3(0.5, 0.5, 0.5) + 0.4*sin(vec3(1.)*explNum+vec3(0.,2.1,-2.1));
    
        // Number of particles
        float N_LIGHTS = 100.;
        for(float i=0.; i<N_LIGHTS; i++)
        {
            // Generate points uniformly on hemisphere
            // (see Total Compendium eq. (34))
            float f = i/N_LIGHTS;
            float r = sqrt(1. - f*f);
            float th = 2.*0.618033*3.14159*i; // Use Golden Ratio for a quasirandom sequence
            float hash = sin(explNum+i*85412.243);
            float weight = (1.-0.2*hash);
            th += hash *3.* 6.28/N_LIGHTS;
            // Only take x and y coordinates
            vec2 lpos = vec2(cos(th), sin(th)) * (r / 3.0);
            // Add some physics
            lpos.xy *= (1.-exp(-3.*t/weight)) * weight; // explosion, easing out
            lpos.y += t*0.3*weight - t*(1.-exp(-t*weight)) * 0.6 * weight; // vertical free-fall motion
            float intensity = 2e-4;
            intensity *= exp(-2.*t); // Fade out with time
            intensity *= (1.-0.5*hash); // Randomize per particle
            intensity *= (1.+10.*exp(-20.*t)); // Intensity burst at explosion
            intensity *= clamp(3.*tFadeout, 0., 1.); // Fade out before next explosion
            col += glow(p, lpos) * intensity * baseCol;
        }
     
        col = max(col, 0.03);
        //col = 1.-exp(-col); // Tone mapping
        col = (col*(2.51*col+0.03))/(col*(2.43*col+0.59)+0.14); // Tone mapping
        //col = col/(1.+col);
        col = sqrt(col); // gamma correction
        gl_FragColor = vec4(col,1.0);
    }
}