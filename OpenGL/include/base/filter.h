
#include "decl.h"
#include "type.h"


namespace base
{
    /// Base filter class.
    class LIBENIGHT_EXPORT Filter
    {
    public:
        Filter(float width);
        virtual ~Filter();

        float width() const { return m_width; }
        float sampleDelta(float x, float scale) const;
        float sampleBox(float x, float scale, int samples) const;
        float sampleTriangle(float x, float scale, int samples) const;

        virtual float evaluate(float x) const = 0;

    protected:
        const float m_width;
    };

    // Box filter.
    class BoxFilter : public Filter
    {
    public:
        BoxFilter();
        BoxFilter(float width);
        virtual float evaluate(float x) const;
    };

    // Triangle (bilinear/tent) filter.
    class TriangleFilter : public Filter
    {
    public:
        TriangleFilter();
        TriangleFilter(float width);
        virtual float evaluate(float x) const;
    };

    // Quadratic (bell) filter.
    class QuadraticFilter : public Filter
    {
    public:
        QuadraticFilter();
        virtual float evaluate(float x) const;
    };

    // Cubic filter from Thatcher Ulrich.
    class CubicFilter : public Filter
    {
    public:
        CubicFilter();
        virtual float evaluate(float x) const;
    };

    // Cubic b-spline filter from Paul Heckbert.
    class BSplineFilter : public Filter
    {
    public:
        BSplineFilter();
        virtual float evaluate(float x) const;
    };

    /// Mitchell & Netravali's two-param cubic
    /// @see "Reconstruction Filters in Computer Graphics", SIGGRAPH 88
    class MitchellFilter : public Filter
    {
    public:
        MitchellFilter();
        virtual float evaluate(float x) const;

        void setParameters(float b, float c);

    private:
        float p0, p2, p3;
        float q0, q1, q2, q3;
    };

    // Lanczos3 filter.
    class LanczosFilter : public Filter
    {
    public:
        LanczosFilter();
        virtual float evaluate(float x) const;
    };

    // Sinc filter.
    class SincFilter : public Filter
    {
    public:
        SincFilter(float w);
        virtual float evaluate(float x) const;
    };

    // Kaiser filter.
    class LIBENIGHT_EXPORT KaiserFilter : public Filter
    {
    public:
        KaiserFilter(float w);
        virtual float evaluate(float x) const;

        void setParameters(float a, float stretch);

    private:
        float alpha;
        float stretch;
    };

    // Gaussian filter.
    class GaussianFilter : public Filter
    {
    public:
        GaussianFilter(float w);
        virtual float evaluate(float x) const;

        void setParameters(float variance);

    private:
        float variance;
    };



    /// A 1D kernel. Used to precompute filter weights.
    class Kernel1
    {
        //NV_FORBID_COPY(Kernel1);
    public:
        Kernel1(const Filter & f, int iscale, int samples = 32);
        ~Kernel1();

        float valueAt(math::uint x) const {
          //  nvDebugCheck(x < (math::uint)m_windowSize);
            return m_data[x];
        }

        int windowSize() const {
            return m_windowSize;
        }

        float width() const {
            return m_width;
        }

        void debugPrint();

    private:
        int m_windowSize;
        float m_width;
        float * m_data;
    };


    /// A 2D kernel.
    class Kernel2 
    {
    public:
        Kernel2(math::uint width);
        Kernel2(math::uint width, const float * data);
        Kernel2(const Kernel2 & k);
        ~Kernel2();

        void normalize();
        void transpose();

        float valueAt(math::uint x, math::uint y) const {
            return m_data[y * m_windowSize + x];
        }

		math::uint windowSize() const {
            return m_windowSize;
        }

        void initLaplacian();
        void initEdgeDetection();
        void initSobel();
        void initPrewitt();

        void initBlendedSobel(const math::V4f & scale);

    private:
        const math::uint m_windowSize;
        float * m_data;
    };


    /// A 1D polyphase kernel
    class LIBENIGHT_EXPORT PolyphaseKernel
    {
       // NV_FORBID_COPY(PolyphaseKernel);
    public:
        PolyphaseKernel(const Filter & f, math::uint srcLength, math::uint dstLength, int samples = 32);
        ~PolyphaseKernel();

        int windowSize() const {
            return m_windowSize;
        }

		math::uint length() const {
            return m_length;
        }

        float width() const {
            return m_width;
        }

        float valueAt(math::uint column, math::uint x) const {
            //nvDebugCheck(column < m_length);
            //nvDebugCheck(x < (uint)m_windowSize);
            return m_data[column * m_windowSize + x];
        }

        void debugPrint() const;

    private:
        int m_windowSize;
		math::uint m_length;
        float m_width;
        float * m_data;
    };

} // nv namespace
