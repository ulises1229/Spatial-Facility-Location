class Point
{

	// This assert is not correct when the floating point model
    // is not IEEE-compliant, float is not 32-bit, or both.
    //
    // Such things are hard to find, so we'll just assume
    // mostly-sane hardware.
    //
    static_assert(
        (sizeof(int) == sizeof(float)) &&
        (sizeof(int)*CHAR_BIT == 32) &&
        (sizeof(long long)*CHAR_BIT == 64),
        "We need 32-bit ints and floats, and 64-bit long longs!"
        );


private:

    float m_x , m_y;



public:
	Point (float x, float y): m_x(x), m_y(y) {}
	Point () {}

    // So we don't lose any information, we need 2x the width.
    // After all, we're cramming two 32-bit numbers into a single value.
    // Lossiness here would mean a container would need to implement
    // a binning strategy.
    //
    // Higher dimensions would require an array, obviously.
    //
    // Also, we're not going to modify the point, so make this a const routine.
    //
    inline long long mortonIndex(float x, float y )
    {
		m_x = x;
		m_y = y;
        // Pun the x and y coordinates as integers: Just re-interpret the bits.
        //
        auto ix = reinterpret_cast<const unsigned &>(this->m_x);
        auto iy = reinterpret_cast<const unsigned &>(this->m_y);

        // Since we're assuming 2s complement arithmetic (99.99% of hardware today),
        // we'll need to convert these raw integer-punned floats into
        // their corresponding integer "indices".

        // Smear their sign bits into these for twiddling below.
        //
        const auto ixs = static_cast<int>(ix) >> 31;
        const auto iys = static_cast<int>(iy) >> 31;

        // This is a combination of a fast absolute value and a bias.
        //
        // We need to adjust the values so -FLT_MAX is close to 0.
        //
        ix = (((ix & 0x7FFFFFFFL) ^ ixs) - ixs) + 0x7FFFFFFFL;
        iy = (((iy & 0x7FFFFFFFL) ^ iys) - iys) + 0x7FFFFFFFL;

        // Now we have -FLT_MAX close to 0, and FLT_MAX close to UINT_MAX,
        // with everything else in-between.
        //
        // To make this easy, we'll work with x and y as 64-bit integers.
        //
        long long xx = ix;
        long long yy = iy;

        // Dilate and combine as usual...

        xx = (xx | (xx << 16)) & 0x0000ffff0000ffffLL;
        yy = (yy | (yy << 16)) & 0x0000ffff0000ffffLL;

        xx = (xx | (xx <<  8)) & 0x00ff00ff00ff00ffLL;
        yy = (yy | (yy <<  8)) & 0x00ff00ff00ff00ffLL;

        xx = (xx | (xx <<  4)) & 0x0f0f0f0f0f0f0f0fLL;
        yy = (yy | (yy <<  4)) & 0x0f0f0f0f0f0f0f0fLL;

        xx = (xx | (xx <<  2)) & 0x3333333333333333LL;
        yy = (yy | (yy <<  2)) & 0x3333333333333333LL;

        xx = (xx | (xx <<  1)) & 0x5555555555555555LL;
        yy = (yy | (yy <<  1)) & 0x5555555555555555LL;

        return xx | (yy << 1);
    }
};
