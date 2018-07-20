

float3 split_3bytes (float v)
{
	v *= (256.0*256.0*256.0 - 1);
	float3 r;
	r.x = floor(v / 256.0 / 256.0); // most significant
	r.y = floor((v - r.x * 256.0 * 256.0) / 256.0);  // middle significant
	r.z = (v - r.x * 256.0 * 256.0 - r.y * 256.0);  // least significant
	return r / 255.0;
}

float join_3bytes (float3 bytes)
{
	return 255.0 * (256.0*256.0*bytes.x + 256.0*bytes.y + bytes.z) / (256.0*256.0*256.0 - 1);
}