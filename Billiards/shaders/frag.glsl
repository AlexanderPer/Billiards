#version 120

uniform vec3 iResolution;
uniform vec3 eye;
uniform sampler2D ballTexture;
uniform float tableSize;
uniform vec2 ballPos;
uniform float ballRadius;
uniform vec3 ballUpVec;
uniform vec3 ballRightVec;
uniform float antialiasing;

#define MAX_STEPS 64
#define EPSILON .001

#define M_PI 3.14159265358979

const float MaxDistance = 80.;
const vec3 FadeLightSource = vec3(4., 4., -2.);
const vec3 LightSource2 = vec3(-10., 10., 10.);

//const vec3 TableColor = vec3(0., 0.8, 0.);
const vec3 TableColor = vec3(0.42, 0.57, 0.14); //Olive Drab
const vec3 BarrierColor = vec3(0.55, 0.27, 0.08); // Saddle Brown
//const vec3 RoomColor = vec3(0.8, 0.52, 0.25); // Peru
//const vec3 RoomColor = vec3(0.74, 0.72, 0.42); // Dark Khaki
const vec3 RoomColor = vec3(0.37, 0.36, 0.21);
const vec3 DoorColor = vec3(0.);
const vec3 WinColor = vec3(2.);

const float TableHeight = 0.0;
const float BarrierThickness = 0.8;
const float RoomRadius = 20.;

const float AAC = 0.25; // antialiasing coefficient

const float BallReflectivity = 0.3;

float HalfSizeTable = tableSize / 2;
vec3 BallCenter = vec3(ballPos.x, TableHeight + ballRadius, ballPos.y);

struct MaterialInfo {
	vec3 Kd;
	float Shininess;
};

float TableSurface(vec3 a) {
	vec3 sz = vec3(HalfSizeTable, 0, HalfSizeTable);
	return length(max(abs(a) - sz, 0.));
}

float Box(vec3 a, vec3 sz) {
	return length(max(abs(a) - sz, 0.));
}

float Barrier(vec3 a) {
	float ht = BarrierThickness / 2;
	float bx1 = Box(a + vec3(HalfSizeTable + ht, -ht, 0), vec3(ht, ht, HalfSizeTable + BarrierThickness));
	float bx2 = Box(a + vec3(-HalfSizeTable - ht, -ht, 0), vec3(ht, ht, HalfSizeTable + BarrierThickness));
	float bx3 = Box(a + vec3(0, -ht, HalfSizeTable + ht), vec3(HalfSizeTable + BarrierThickness, ht, ht));
	float bx4 = Box(a + vec3(0, -ht, -HalfSizeTable - ht), vec3(HalfSizeTable + BarrierThickness, ht, ht));
	return min(min(bx1, bx2), min(bx3, bx4));
}

float Room(vec3 a) {
	float wallSize = 60;
	float hs = wallSize / 2;
	float floorDist = wallSize / 8;
 	float wl1 = Box(a + vec3(RoomRadius, -hs + floorDist, 0), vec3(0.1, hs, hs));
	float wl2 = Box(a + vec3(-RoomRadius, -hs + floorDist, 0), vec3(0.1, hs, hs));
	float wl3 = Box(a + vec3(0, -hs + floorDist, RoomRadius), vec3(hs, hs, 0.1));
	float wl4 = Box(a + vec3(0, -hs + floorDist, -RoomRadius), vec3(hs, hs, 0.1));
	float cl = Box(a + vec3(0, -wallSize + floorDist, 0), vec3(hs, 0.1, hs));
	float fl = Box(a + vec3(0, floorDist, 0), vec3(hs, 0.1, hs));
	return min(min( min(wl1, wl2), min(wl3, wl4) ), min(cl, fl));
}

float Doors(vec3 a) {
	float size = 25;
	float floorDist = 10;
	float hs = size / 2;
	float dr1 = Box(a + vec3(0, -hs + floorDist, RoomRadius), vec3(hs / 4, hs, 0.2));
	float dr2 = Box(a + vec3(0, -hs + floorDist, -RoomRadius), vec3(hs / 4, hs, 0.2));
	return min(dr1, dr2);
}

float Windows(vec3 a) {
	float size = 20;
	float floorDist = 5;
	float hs = size / 2;
	float w1 = Box(a + vec3(RoomRadius, -hs - floorDist, 0), vec3(0.15, hs, hs / 6));
	float w2 = Box(a + vec3(-RoomRadius, -hs - floorDist, 0), vec3(0.15, hs, hs / 6));
	return min(w1, w2);
}

float Ball(vec3 a) {
	return length(a - BallCenter) - 1.;
}

float Scene(vec3 a) {
	return min( min( min(Ball(a), TableSurface(a)), min(Room(a), Barrier(a)) ), min(Doors(a), Windows(a)));
}

vec3 BallTextureColor(vec3 a) {
	vec3 rVec = normalize(ballRightVec);
	vec3 uVec = normalize(ballUpVec);
	vec3 eVec = normalize(cross(rVec, uVec)); // eye vec

	vec3 radVec = normalize(a - BallCenter);
	float angle1 = acos(-dot(radVec, uVec)) / M_PI;

	vec3 N = uVec;    // plane normal
	float K = -dot(N, radVec);
	vec3 prRadVec = radVec + K * N;  // equator plan projection
	vec3 equatorRadVec = normalize(prRadVec);

	float semiSphere = dot(equatorRadVec, eVec);
	float angle2 =  acos(-dot(equatorRadVec, rVec)) / M_PI / 2;
	if (semiSphere < 0)
		angle2 = acos(dot(equatorRadVec, rVec)) / M_PI / 2 + 1/2;

	return texture2D(ballTexture, vec2(angle2, angle1)).rgb;
}

bool ballReflection = false;
// material for specified point
MaterialInfo Material(vec3 a) {
	MaterialInfo m = MaterialInfo(vec3(0.), 0.);
	float closest = MaxDistance;

	float roomDistance = Room(a);
	if (roomDistance < closest) {
		m.Kd = RoomColor;
		m.Shininess = 20.;
		closest = roomDistance;
	}

	float doorDistance = Doors(a);
	if (doorDistance < closest) {
		m.Kd = DoorColor;
		m.Shininess = 0.;
		closest = doorDistance;
	}

	float winDistance = Windows(a);
	if (winDistance < closest) {
		m.Kd = WinColor;
		m.Shininess = 0.;
		closest = winDistance;
	}
	
	float tableDistance = TableSurface(a);
	if (tableDistance < closest) {
		m.Kd = TableColor;
		m.Shininess = 20.;
		closest = tableDistance;
	}

	float barrierDistance = Barrier(a);
	if (barrierDistance < closest) {
		m.Kd = BarrierColor;
		m.Shininess = 80.;
		closest = barrierDistance;
	}

	float ballDistance = Ball(a);
	if ((ballDistance < closest) && (!ballReflection)) {
		m.Kd = BallTextureColor(a);
		m.Shininess = 150.;
		ballReflection = true;	
	}
	return m;
}

// normal = gradient
vec3 Normal(vec3 a) {
	vec2 e = vec2(.001, 0.);
	float s = Scene(a);
	return normalize(vec3(
		Scene(a+e.xyy) - s,
		Scene(a+e.yxy) - s,
		Scene(a+e.yyx) - s));
}

// shading nearby objects (coefficient)
float Occlusion(vec3 at, vec3 normal) {
	// shading = 0
	float b = 0.;
	// 4 steps
	for (int i = 1; i <= 4; ++i) {
		// .06 - step distance (can be tuned)
		float L = .06 * float(i);
		float d = Scene(at + normal * L);		
		// add difference between the distances traversed and the minimum
		b += max(0., L - d);
	}
	// coefficient <= 1
	return min(b, 1.);
}

vec3 LookAt(vec3 pos, vec3 at, vec3 rDir) {
	vec3 f = normalize(at - pos);
	vec3 r = cross(f, vec3(0., 1., 0.));
	vec3 u = cross(r, f);
	return mat3(r, u, -f) * rDir;
}

// ray tracing function from rPos, rDir - ray direction
float Trace(vec3 rPos, vec3 rDir, float distMin) {
	float L = distMin;
	for (int i = 0; i < MAX_STEPS; ++i) {
		// distance to the nearest object
		float d = Scene(rPos + rDir * L);
		L += d;
		// check threshold value
		if (d < EPSILON * L) break;
	}
	// traveled distance
	return L;
}

vec3 Lighting(vec3 at, vec3 normal, vec3 eye, MaterialInfo m, vec3 lColor, vec3 lPos) {
	// direction to light source from current point
	vec3 lDir = lPos - at;
	
	// throw ray to light source (barriers check)
	vec3 lDirN = normalize(lDir);
	float t = Trace(at, lDirN, EPSILON*2.);
	if (t < length(lDir)) {
		return vec3(0.);
	}
	vec3 color = m.Kd * lColor * max(0., dot(normal, normalize(lDir)));
	
	if (m.Shininess > 0.) {
		// Blinn–Phong reflection model (calculates a halfway vector between the viewer and light-source vectors)
		vec3 h = normalize(normalize(lDir) + normalize(eye - at));
		// normalizing in end
		color += lColor * pow(max(0., dot(normal, h)), m.Shininess) * (m.Shininess + 8.) / 25.;
	}
	return color;
}

vec3 FadeLighting(vec3 at, vec3 normal, vec3 eye, MaterialInfo m) {	
	vec3 color = Lighting(at, normal, eye, m, vec3(1.2), FadeLightSource);
	vec3 lDir = FadeLightSource - at;
	// divide by square of distance to light source (5 is tune)
	return 5 * color / dot(lDir, lDir);
}

// rpos - ray position, rdir - ray direction, t - path
vec3 Shade(vec3 rpos, vec3 rdir, float t, vec3 pos, vec3 nor)
{	
	// current point material
	MaterialInfo mat = Material(pos);

	// ambient light
	vec3 color = .11 * (1. - Occlusion(pos, nor)) * mat.Kd;

	// light from light source
	color += FadeLighting(pos, nor, rpos, mat);
	color += Lighting(pos, nor, rpos, mat, vec3(0.6), LightSource2);

	return color;
}


vec3 ShadeAndReflect(vec3 rpos, vec3 rdir, float t)
{
	// moving end result
	vec3 pos = rpos + rdir * t;
	vec3 nor = Normal(pos);

	vec3 color = Shade(rpos, rdir, t, pos, nor);

	if (ballReflection)
	{
		rdir = normalize(reflect(rdir, nor));
		rpos = pos + rdir * EPSILON * 2.;
		
		float t = Trace(rpos, rdir, 0.);

		pos = rpos + rdir * t;
		nor = Normal(pos);
		color += Shade(rpos, rdir, t, pos, nor) * BallReflectivity;

		ballReflection = false;
	}
	
	return color;
}

// Ray-generation
vec3 Camera(vec2 px) {
	vec2 uv = px / iResolution.xy * 2. - 1.;	
	uv.x *= iResolution.x / iResolution.y;
	vec3 rayStart = eye;
	vec3 rayDirection = LookAt(rayStart, vec3(0, -1, 0), normalize(vec3(uv, -2.)));
	// calculate traveled distance
	float path = Trace(rayStart, rayDirection, 0.);	
	return ShadeAndReflect(rayStart, rayDirection, path);
}

void main(void) {	
	//gl_FragColor = vec4(texture2D(ballTexture, gl_FragCoord.xy/iResolution.xy).rgb, 1);
	
	vec3 col = Camera(gl_FragCoord.xy);

	if (antialiasing > 0.5 ) {
		col += Camera(gl_FragCoord.xy + vec2(AAC, AAC));
		col += Camera(gl_FragCoord.xy + vec2(-AAC, AAC));
		col += Camera(gl_FragCoord.xy + vec2(AAC, -AAC));
		col += Camera(gl_FragCoord.xy + vec2(-AAC, -AAC));
		col /= 5;
	}
	gl_FragColor = vec4(col, 0.);
}
