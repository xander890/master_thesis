#include "miscellaneous.h"
#include "CGLA/Mat2x2f.h"
using namespace CGLA;
using namespace std;

CGLA::Vec3f refract(const CGLA::Vec3f &in, const CGLA::Vec3f & n, float n1, float n2)
{
    float eta = n1/n2;
    float c = dot(in, n);
    return eta * (c * n - in) - n * sqrt(1 - eta * eta * (1 - c * c));
}


Vec2f fresnelAmplitudeReflectance(const CGLA::Vec3f &in, const CGLA::Vec3f &n, float nin, float ntr)
{
    float cosin = dot(in,n);
    Vec3f refr = refract(in,n,nin,ntr);
    float costr = dot(refr,-n);

    float r_s = (nin * cosin - ntr * costr) / (nin * cosin + ntr * costr);
    float r_p = (nin * costr - ntr * cosin) / (nin * costr + ntr * cosin);
    //std::cout << abs(r_s) << " ¤ " << abs(r_p) << std::endl;
    return Vec2f(r_s,r_p);
}


Vec2f fresnelPowerReflectance(const Vec3f &in, const Vec3f &n, float n1, float n2)
{
    Vec2f r = fresnelAmplitudeReflectance(in,n,n1,n2);
    return r * r;
}


Vec2f fresnelAmplitudeTransmittance(const Vec3f &in, const Vec3f &n, float n1, float n2)
{
    float cosin = dot(in,n);
    Vec3f refr = refract(in,n,n1,n2);
    float costr = dot(refr,-n);
    //std::cout << n1 *  sqrt(1 - cosin * cosin) << " = " << n2 * sqrt(1 - costr * costr) << std::endl;
    float t_s = (2 * n1 * cosin) / (n1 * cosin + n2 * costr);
    float t_p = (2 * n1 * cosin) / (n1 * costr + n2 * cosin);
    //std::cout << abs(t_s) << " # " << abs(t_p) << std::endl;
    return Vec2f(t_s,t_p);
}


Vec2f fresnelPowerTransmittance(const Vec3f &in, const Vec3f &n, float n1, float n2)
{
    float cosin = dot(in,n);
    Vec3f refr = refract(in,n,n1,n2);
    float costr = dot(refr,-n);

    Vec2f t = fresnelAmplitudeTransmittance(in,n,n1,n2);

    return ((n2 * costr) / (n1 * cosin)) * (t * t);
}

float C_1(float ni)
{
    float c;
    if(ni < 1.0f)
    {
        c = + 0.919317
            - 3.4793  * ni
            + 6.75335 * pow(ni,2)
            - 7.80989 * pow(ni,3)
            + 4.98554 * pow(ni,4)
            - 1.36881 * pow(ni,5);
    }
    else
    {
        c = - 9.23372
            + 22.2272  * ni
            - 20.9292  * pow(ni,2)
            + 10.2291  * pow(ni,3)
            - 2.54396  * pow(ni,4)
            + 0.254913 * pow(ni,5);
    }
    return c * 0.5f;
}

float C_Sigma(float ni)
{

    return 0.25 * (1 - 2 * C_1(ni));
}


float C_2(float ni)
{
    float c;
    if(ni < 1.0f)
    {
        c = + 0.828421
            - 2.62051  * ni
            + 3.36231  * pow(ni , 2)
            - 1.95284  * pow(ni , 3)
            + 0.236494 * pow(ni , 4)
            + 0.145787 * pow(ni,5);
    }
    else
    {
        c = - 1641.1
            + 135.926 / pow(ni,3)
            - 656.175 / pow(ni,2)
            + 1376.53 / ni
            + 1213.67 * ni
            - 568.556 * pow(ni,2)
            + 164.798 * pow(ni,3)
            - 27.0181 * pow(ni,4)
            + 1.91826 * pow(ni,5);
    }
    return c * (1.0f/3.0f);
}

float C_e(float ni)
{
    return 0.5 * (1 - 3 * C_2(ni));
}

void debugFresnelTerms()
{
    int steps = 50;
    for(int i = 0; i < steps; i++)
    {
        Vec3f normal = Vec3f(0,0,1);
        float angle = (i / ((float)steps - 1)) * M_PI / 2;
        Vec3f incident = Vec3f(sin(angle), 0 , cos(angle));
        float n1 = 1.0f;
        float n2 = 1.5f;

        float T = fresnel_T(incident,normal,n1,n2);

        float R = fresnel_R(incident,normal,n1,n2);
        Vec2f T1 = fresnelPowerReflectance(incident,normal,n1,n2);
        Vec2f R1 = fresnelPowerTransmittance(incident,normal,n1,n2);
        Vec2f c (T,R);
        std::cout << "*Fresnel test: n1 = " << n1 << ", n2 = " << n2 << ", dot = " << dot(incident,normal) << " (t,T,r,R) = " << c << ", R+T = " << (T+R) << std::endl;
    }
}



float clamp01(float x)
{
    return clamp(x,0.0f,1.0f);
}


float clamp(float x, float min_f, float max_f)
{
    return std::max(std::min(x,max_f),min_f);
}


int clamp(int x, int min_i, int max_i)
{
    return std::max(std::min(x,max_i),min_i);
}

void planeHammersley(std::vector<Vec2f> &result, int n)
{
    float p, u, v;
    int k, kk;
    for (k=0; k<n; k++)
    {
        u = 0;
        for (p=0.5, kk=k ; kk ; p*=0.5, kk>>=1)
            if (kk & 1) // kk mod 2 == 1
                u += p;
        v = (k + 0.5) / n;
        result.push_back(Vec2f(u,v));
    }
}

void planeHammersleyCircleAlternative(std::vector<Vec2f> &result, int n)
{
   vector<Vec2f> intermediate;
   planeHammersley(intermediate,n);
   for(int i = 0; i < intermediate.size(); i++)
   {
       Vec2f tmp = 2*intermediate[i] - Vec2f(1.0f); //map from [0,1]x[0,1] to [-1,1]x[-1,1]
       Vec2f t = Vec2f(tmp[0] * sqrt(1 - 0.5 * tmp[1]*tmp[1]), tmp[1] * sqrt(1 - 0.5 * tmp[0]*tmp[0]));
       result.push_back(t);
   }
}

void planeHammersleyCircle(std::vector<Vec2f> &result, int n)
{
    //Better method based on hemisphere
   vector<Vec2f> intermediate;
   planeHammersley(intermediate,n);
   for(int i = 0; i < intermediate.size(); i++)
   {
       float x = intermediate[i][0];
       float y = intermediate[i][1];
       float phi = 2 * M_PI * x;
       float costheta = 1 - y;
       float sintheta = sqrt(1 - costheta * costheta);
       Vec2f t = Vec2f(sintheta * cos(phi), sintheta * sin(phi));
       result.push_back(t);
   }
}

void planeHaltonCircleRejectionExponential(std::vector<Vec2f> &result, int n, float sigma_tr, float radius)
{
    //Better method based on hemisphere
   uint accepted = 0;

   gel_srand(0);
   int i = 1;
   sigma_tr = 21.0f;
   float D = 0.0138f;
   while(accepted < n)
   {
        Vec2f point = haltonPointCircle(i, 2, 3);
        float rad = point.length() * radius;
        Vec3f refr = refract(normalize(Vec3f(1,1,1) - Vec3f(point[0],point[1],0)),Vec3f(0,0,1),1.0f,1.3f);
        float cosp = dot(refr, normalize(Vec3f(point[0],point[1],0)));
        float expon = D * sigma_tr * sigma_tr * 0.5 *
                (( rad * rad  * (1.0f/D + 3 * sigma_tr * rad * cosp) + 3 * rad * cosp)
                   * exp(-1 * (sigma_tr * rad)));
        float zeta = gel_rand() / ((float)(GEL_RAND_MAX));
        if(zeta < expon)
        {
            result.push_back(radius * point);
            accepted++;
        }
        i++;
   }
   //cout << "Points, halton" << endl;
   //for(int i = 0; i < n; i++) cout << result[i][0] << " " << result[i][1] << endl;
   cout << "Acceptance rate: " << (float)n / i << endl;
}

void planeHammersleyCircleMulti(vector<vector<Vec2f> > &result, int n, int cols)
{
    gel_srand(0);
    vector<Vec2f> intermediate;
    planeHammersleyCircle(intermediate,n);
    gel_rand();
    for(int k = 0; k < cols; k++)
    {
        float angle = ((float)k) / cols * 2 * M_PI;
        Mat2x2f rot = Mat2x2f(cos(angle),sin(angle), -sin(angle), cos(angle));

        vector<Vec2f> * vec = new vector<Vec2f>(n);
        for(int i = 0; i < n; i++)
        {
            (*vec)[i] = rot * intermediate[i];
        }
        result.push_back(*vec);
    }
}

void circleUniformPoints(vector<vector<Vec2f> > &result, int n, int cols, int m)
{
    for(int k = 0; k < cols; k++)
    {
        //float angle = ((float)k) / cols * 2 * M_PI;
        //Mat2x2f rot = Mat2x2f(cos(angle),sin(angle), -sin(angle), cos(angle));

        vector<Vec2f> * vec = new vector<Vec2f>(n*m);
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < m; j++)
            {
                float angle = ((float)j) / m * 2 * M_PI;
                float radius = ((float)(i + 1)) / (n);
                (*vec)[i * m + j] = Vec2f(radius * cos(angle), radius * sin(angle));
            }
        }
        result.push_back(*vec);
    }
}


void planeHammersleyCircleMultiExp(std::vector<std::vector<Vec2f> > &result, int n, int cols, float sigma)
{
    vector<Vec2f> intermediate;
    planeHammersleyCircle(intermediate,n);
    gel_rand();
    for(int k = 0; k < cols; k++)
    {
        float angle = ((float)k) / cols * 2 * M_PI;
        Mat2x2f rot = Mat2x2f(cos(angle),sin(angle), -sin(angle), cos(angle));

        vector<Vec2f> * vec = new vector<Vec2f>(n);
        for(int i = 0; i < n; i++)
        {
            Vec2f uv = intermediate[i];
            float polar_r = sqrt(uv[0]*uv[0] + uv[1]*uv[1]);
            float polar_theta = atan2(uv[1],uv[0]);


            polar_r = (exp(sigma * polar_r) - 1)/(exp(sigma) - 1);

            uv[0] = polar_r * cos(polar_theta);
            uv[1] = polar_r * sin(polar_theta);

            (*vec)[i] = rot * uv;
        }
        result.push_back(*vec);
    }
}

float vanDerCorputPoint(int n, int basis)
{
    int kp = n;
    float pp = (float)basis;
    float phi = 0.0f;
    while(kp > 0)
    {
        int a = kp % basis;
        phi = phi + a / pp;
        kp = int(kp / basis);
        pp = pp * basis;
    }
    return phi;
}


Vec2f haltonPoint(int k, int p1, int p2)
{
    return Vec2f(vanDerCorputPoint(k,p1),vanDerCorputPoint(k,p2));
}


void planeHalton(std::vector<Vec2f> &result, int n)
{
    for(int i = 0; i < n; i++)
    {
        result.push_back(haltonPoint(i+1,2,3));
    }
}


Vec2f haltonPointCircle(int k, int p1, int p2)
{
    Vec2f p = haltonPoint(k,p1,p2);
    float x = p[0];
    float y = p[1];
    float phi = 2 * M_PI * y;
    float radius = sqrt(x);
    //Vec3f sph = haltonPointSphere(k,p1,p2);
    return Vec2f(radius * cos(phi), radius * sin(phi));
}



Vec3f haltonPointSphere(int k, int p1, int p2)
{
    Vec2f p = haltonPoint(k,p1,p2);
    float x = p[0];
    float y = p[1];
    float phi = 2 * M_PI * x;
    float costheta = (1.0f - y) * 2.0f - 1.0f;
    float sintheta = sqrt(1 - costheta * costheta);
    return Vec3f(sintheta * cos(phi), sintheta * sin(phi),costheta);
}


void planeHaltonCircle(std::vector<Vec2f> &result, int n)
{
    for(int i = 0; i < n; i++)
    {
        result.push_back(haltonPointCircle(i+1,2,3));
    }
}


void planeHaltonCircleRejectionExponentialMulti(std::vector<std::vector<Vec2f> > &result, int n, int cols, float sigma_tr, float radius)
{
    vector<Vec2f> intermediate;
    planeHaltonCircleRejectionExponential(intermediate,n,sigma_tr,radius);
    for(int k = 0; k < cols; k++)
    {
        float angle = ((float)k) / cols * 2 * M_PI;
        Mat2x2f rot = Mat2x2f(cos(angle),sin(angle), -sin(angle), cos(angle));

        vector<Vec2f> * vec = new vector<Vec2f>(n);
        for(int i = 0; i < n; i++)
        {
            (*vec)[i] = rot * intermediate[i];
        }
        result.push_back(*vec);
    }
}

void planeHaltonCircleUniformMulti(std::vector<std::vector<Vec2f> > &result, int n, int cols, float sigma_tr, float radius)
{
    vector<Vec2f> intermediate;
    for(int i = 1; i < (n + 1); i++)
    {
        intermediate.push_back(radius * haltonPointCircle(i,2,3));
    }
    for(int k = 0; k < cols; k++)
    {
        float angle = ((float)k) / cols * 2 * M_PI;
        Mat2x2f rot = Mat2x2f(cos(angle),sin(angle), -sin(angle), cos(angle));

        vector<Vec2f> * vec = new vector<Vec2f>(n);
        for(int i = 0; i < n; i++)
        {
            (*vec)[i] = rot * intermediate[i];
        }
        result.push_back(*vec);
    }
}



void sphereHalton(std::vector<Vec3f> &result, int n)
{
    for(int i = 0; i < n; i++)
    {
        result.push_back(haltonPointSphere(i+1,2,3));
    }
}

void print_memory_info_nvidia()
{
    GLint data[5];
    const char * messages[5] = {"Dedicated video memory: %1 Mb", "Total available memory: %1 Mb", "Current available video memory: %1 Mb", "Eviction count: %1 ", "Evicted memory: %1 Mb"};
    const int divisors[5] = {1024,1024,1024,1,1024};
    glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &data[0]);
    glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &data[1]);
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &data[2]);
    glGetIntegerv(GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &data[3]);
    glGetIntegerv(GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &data[4]);
    for(int i = 0; i < 5; i++)
        cout << QString(messages[i]).arg(data[i] / divisors[i]).toStdString() << endl;
    check_gl_error();
}

float exponential_over_cube_approximation(float sigma_tr, float min_radius)
{
    float a = min_radius;
    float t1 = sigma_tr * a;
    float t2 = exp(-t1);
    float t7 = sqrt(t1);
    float t11 = sqrt(0.806e3);
    float t13 = pow(t1, (float)(t11 / 0.26e2));
    float t21 = pow(0.1e1 / (t1 * t7 + 0.1e1) + 0.4596323387e0 * t13 / (0.1e1 + 0.20e2 / 0.47e2 * t13) + 0.1042076494e1 * t1, 0.2e1);
    float t25 = log(0.1e1 + 0.5614594836e0 / sigma_tr / a - 0.4385405164e0 / t21);
    float t28 = exp(-0.2280291017e1 * t1);
    float t32 = sigma_tr * sigma_tr;
    float t34 = a * a;
    return (t2 * t25 / (0.5614594836e0 + 0.4385405164e0 * t28) * t32 * t34 - t2 * a * sigma_tr + t2) / t34 / 0.2e1;

}
