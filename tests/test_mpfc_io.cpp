// SPDX-License-Identifier: BSD-2-Clause

#include <cassert>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

#include <gmpxx_mkII.h>

namespace {

class test_numpunct : public std::numpunct<char> {
public:
    explicit test_numpunct(char decimal_point)
        : decimal_point_(decimal_point) {}

protected:
    char do_decimal_point() const override {
        return decimal_point_;
    }

private:
    char decimal_point_;
};

void test_output_uses_complex_pair_format() {
    using namespace gmpxx;

    mpfc_class z(mpf_class("1.25", 192), mpf_class("-2.5", 160));

    std::ostringstream os;
    os << z;
    assert(os.str() == "(1.25,-2.5)");
}

void test_output_respects_mpf_stream_formatting() {
    using namespace gmpxx;

    mpfc_class z(mpf_class("1.25", 192), mpf_class("-2.5", 160));

    std::ostringstream os;
    os << std::scientific << std::setprecision(3) << z;
    assert(os.str() == "(1.250e+00,-2.500e+00)");
}

void test_output_respects_additional_stream_flags() {
    using namespace gmpxx;

    mpfc_class z(mpf_class("1.25", 192), mpf_class("2.5", 160));

    std::ostringstream os;
    os << std::fixed << std::showpos << std::setprecision(2)
       << std::setw(20) << std::setfill('*') << z;
    assert(os.str() == "(+1.25,+2.50)");
    assert(os.width() == 0);
}

void test_expression_output() {
    using namespace gmpxx;

    mpfc_class lhs(mpf_class(1, 160), mpf_class(2, 160));
    mpfc_class rhs(mpf_class(3, 160), mpf_class(-4, 160));

    std::ostringstream os;
    os << lhs + rhs;
    assert(os.str() == "(4,-2)");
}

void test_input_uses_complex_pair_format() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(0, 96), mpf_class(0, 128));
    mp_bitcnt_t real_prec = z.real().get_prec();
    mp_bitcnt_t imag_prec = z.imag().get_prec();

    std::istringstream is("(1.25,-2.5)");
    is >> z;

    assert(is);
    assert(z.real() == mpf_class("1.25", real_prec));
    assert(z.imag() == mpf_class("-2.5", imag_prec));
    assert(z.real().get_prec() == real_prec);
    assert(z.imag().get_prec() == imag_prec);
}

void test_input_preserves_precision_for_high_precision_values() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(0, 80), mpf_class(0, 96));
    mp_bitcnt_t real_prec = z.real().get_prec();
    mp_bitcnt_t imag_prec = z.imag().get_prec();

    std::istringstream is(
        "(1.2345678901234567890123456789,"
        "-9.8765432109876543210987654321)");
    is >> z;

    assert(is);
    assert(z.real().get_prec() == real_prec);
    assert(z.imag().get_prec() == imag_prec);
    assert(z.real() ==
           mpf_class("1.2345678901234567890123456789", real_prec));
    assert(z.imag() ==
           mpf_class("-9.8765432109876543210987654321", imag_prec));
}

void test_input_allows_whitespace_with_skipws() {
    using namespace gmpxx;

    mpfc_class z;
    std::istringstream is(" ( 1.25 , -2.5 ) ");
    is >> z;

    assert(is);
    assert(z.real() == mpf_class("1.25"));
    assert(z.imag() == mpf_class("-2.5"));
}

void test_failed_input_preserves_existing_value() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 96), mpf_class(4, 128));
    mp_bitcnt_t real_prec = z.real().get_prec();
    mp_bitcnt_t imag_prec = z.imag().get_prec();

    std::istringstream is("(1.25; -2.5)");
    is >> z;

    assert(!is);
    assert(z.real() == 3);
    assert(z.imag() == 4);
    assert(z.real().get_prec() == real_prec);
    assert(z.imag().get_prec() == imag_prec);
}

void test_failed_input_preserves_existing_value_for_late_failures() {
    using namespace gmpxx;

    char const* inputs[] = {
        "(1.25,not-a-number)",
        "(1.25,-2.5]",
        "(1.25,-2.5",
    };

    for (char const* text : inputs) {
        mpfc_class z(mpf_class(3, 96), mpf_class(4, 128));
        mp_bitcnt_t real_prec = z.real().get_prec();
        mp_bitcnt_t imag_prec = z.imag().get_prec();

        std::istringstream is(text);
        is >> z;

        assert(!is);
        assert(z.real() == 3);
        assert(z.imag() == 4);
        assert(z.real().get_prec() == real_prec);
        assert(z.imag().get_prec() == imag_prec);
    }
}

void test_real_only_input_is_not_accepted_initially() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 96), mpf_class(4, 128));
    std::istringstream is("1.25");
    is >> z;

    assert(!is);
    assert(z.real() == 3);
    assert(z.imag() == 4);
}

void test_parenthesized_real_only_input_is_not_accepted() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 96), mpf_class(4, 128));
    std::istringstream is("(1.25)");
    is >> z;

    assert(!is);
    assert(z.real() == 3);
    assert(z.imag() == 4);
}

void test_locale_decimal_point_for_output_and_input() {
    using namespace gmpxx;

    std::locale comma_locale(std::locale::classic(), new test_numpunct(','));

    mpfc_class z(mpf_class("1.25", 192), mpf_class("-2.5", 160));
    std::ostringstream os;
    os.imbue(comma_locale);
    os << z;
    assert(os.str() == "(1,25,-2,5)");

    mpfc_class parsed(mpf_class(0, 96), mpf_class(0, 128));
    std::istringstream is(os.str());
    is.imbue(comma_locale);
    is >> parsed;
    assert(is);
    assert(parsed.real() == mpf_class("1.25", parsed.real().get_prec()));
    assert(parsed.imag() == mpf_class("-2.5", parsed.imag().get_prec()));
}

}  // namespace

int main() {
    test_output_uses_complex_pair_format();
    test_output_respects_mpf_stream_formatting();
    test_output_respects_additional_stream_flags();
    test_expression_output();
    test_input_uses_complex_pair_format();
    test_input_preserves_precision_for_high_precision_values();
    test_input_allows_whitespace_with_skipws();
    test_failed_input_preserves_existing_value();
    test_failed_input_preserves_existing_value_for_late_failures();
    test_real_only_input_is_not_accepted_initially();
    test_parenthesized_real_only_input_is_not_accepted();
    test_locale_decimal_point_for_output_and_input();
    return 0;
}
