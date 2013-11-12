#pragma once

/* A basic geometry class which simply tests against a cornell box, for
 * testing purposes only. This code is horrible. */

#include "math/vector3.hpp"
#include <cmath>

#include "contact.hpp"

#include <algorithm>

struct Plane
{
    math::float3 point;
    math::float3 normal;
};

struct Sphere
{
    math::float3 center;
    float radius;
};

class CornellBox
{
public:
    math::float3 light() const // testing with a single point light source
    {
        return math::float3(-0.3f, -0.2f, 0.35f);
    }
    
    bool occludes(const math::float3 &origin, const math::float3 &direction,
                  float max_dist) const
    {
        float distance;
        Contact contact;
        
        traverse(origin, direction, distance, contact);
        return (distance < max_dist);
    }

	bool traverse(const math::float3 &origin, const math::float3 &direction,
		          float &distance, Contact &contact) const
	{
	    bool plane = true;
	    int index = -1;

		distance = std::numeric_limits<float>::infinity();
	    
	    for (size_t t = 0; t < planes.size(); ++t)
	    {
	        float d = ray_plane(origin, direction, planes[t]);
	        if (d > 0 && d < distance)
	        {
	            distance = d;
	            plane = true;
	            index = t;
	        }
	    }
	    
	    for (size_t t = 0; t < spheres.size(); ++t)
	    {
	        float d = ray_sphere(origin, direction, spheres[t]);
	        if (d > 0 && d < distance)
	        {
	            distance = d;
	            plane = false;
	            index = t;
	        }
	    }
	    
	    if (index == -1) return false;
	    else
	    {
	        if (plane)
	        {
	            switch (index)
	            {
	                case 0: contact.rgb = math::float3(0.55, 0.75, 0.55); break;
	                case 1: contact.rgb = math::float3(1, 1, 1); break;
	                case 2: contact.rgb = math::float3(0.25, 0.25, 0.75); break;
	                case 3: contact.rgb = math::float3(0.75, 0.25, 0.25); break;
	                case 4: contact.rgb = math::float3(1, 1, 1); break;
	            }
	            
	            contact.normal = planes[index].normal;
	        }
	        else
	        {
	            switch (index)
	            {
	                case 0: contact.rgb = math::float3(0.75, 0.75, 0.25); break;
	                case 1: contact.rgb = math::float3(0.25, 0.75, 0.75); break;
	            }
	            
	            contact.normal = normalize(origin + direction * distance - spheres[index].center);
	        }
	        
	        return true;
	    }
	}
	
	CornellBox()
	{
	    planes.push_back(Plane{math::float3(0, -1, 0), math::float3(0, +1, 0)});
        planes.push_back(Plane{math::float3(0, +1, 0), math::float3(0, -1, 0)});
        planes.push_back(Plane{math::float3(-1, 0, 0), math::float3(+1, 0, 0)});
        planes.push_back(Plane{math::float3(+1, 0, 0), math::float3(-1, 0, 0)});
        planes.push_back(Plane{math::float3(0, 0, +1), math::float3(0, 0, -1)});
        
        spheres.push_back(Sphere{math::float3(0.05f, -0.55f, 0.75f), 0.15f});
	}

private:
    std::vector<Plane> planes;
    std::vector<Sphere> spheres;

	static float ray_sphere(const math::float3 &origin,
	                        const math::float3 &direction,
	                        const Sphere &sphere)
	{
	    math::float3 org = sphere.center - origin;
	    float b = dot(org, direction);
	    float det = b * b - dot(org, org) + pow(sphere.radius, 2);

	    if (det >= 0.0f)
		{
			float l = sqrt(det);
			float p = b - l, q = b + l;
			if (p < 1e-5f) return q;
			if (q < 1e-5f) return p;
			return std::min(p, q);
	    }
	    return -1.0f;
	}
	
	static float ray_plane(const math::float3 &origin,
	                       const math::float3 &direction,
	                       const Plane &plane)
	{
	    return math::dot(plane.point - origin, plane.normal) / math::dot(direction, plane.normal);
	}
};
