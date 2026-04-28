// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <stdexcept>
#include <utility>

namespace {

mpf_class two_to_minus(mp_bitcnt_t bits, mp_bitcnt_t prec) {
    mpf_class result(1, prec);
    mpf_div_2exp(result.get_mpf_t(), result.get_mpf_t(), bits);
    return result;
}

mpf_class parse_decimal_literal(char const* text, mp_bitcnt_t precision) {
    mpf_class value(0, precision);
    int rc = mpf_set_str(value.get_mpf_t(), text, 10);
    assert(rc == 0);
    return value;
}

mpf_class rounded_to(mpf_class const& value, mp_bitcnt_t precision) {
    mpf_class result(0, precision);
    mpf_set(result.get_mpf_t(), value.get_mpf_t());
    return result;
}

mpf_class ulp_for_value(mpf_class const& value, mp_bitcnt_t precision) {
    mp_exp_t exponent = 0;
    mpf_get_d_2exp(&exponent, value.get_mpf_t());

    mpf_class ulp(1, precision + 8);
    if (exponent >= static_cast<mp_exp_t>(precision)) {
        mpf_mul_2exp(ulp.get_mpf_t(), ulp.get_mpf_t(),
                     exponent - static_cast<mp_exp_t>(precision));
    } else if (precision < 2) {
        mpf_mul_2exp(ulp.get_mpf_t(), ulp.get_mpf_t(), 2 - precision);
    } else {
        mpf_div_2exp(ulp.get_mpf_t(), ulp.get_mpf_t(),
                     static_cast<mp_bitcnt_t>(
                         static_cast<mp_exp_t>(precision) - exponent));
    }
    return ulp;
}

void assert_close(mpf_class const& got, mpf_class const& expected,
                  mp_bitcnt_t tolerance_bits) {
    mp_bitcnt_t prec = std::max(got.get_prec(), expected.get_prec());
    mpf_class diff(prec);
    mpf_sub(diff.get_mpf_t(), got.get_mpf_t(), expected.get_mpf_t());
    mpf_abs(diff.get_mpf_t(), diff.get_mpf_t());

    mpf_class tolerance = two_to_minus(tolerance_bits, prec);
    assert(diff < tolerance);
}

void assert_within_ulp(mpf_class const& got, mpf_class const& expected,
                       mp_bitcnt_t precision, unsigned long max_ulps = 1) {
    mpf_class diff(precision + 8);
    mpf_set(diff.get_mpf_t(), got.get_mpf_t());
    mpf_sub(diff.get_mpf_t(), diff.get_mpf_t(), expected.get_mpf_t());
    mpf_abs(diff.get_mpf_t(), diff.get_mpf_t());

    mpf_class bound = ulp_for_value(expected, precision);
    mpf_mul_ui(bound.get_mpf_t(), bound.get_mpf_t(), max_ulps);
    assert(diff <= bound);
}

void assert_precision_doubling(mpf_class (*fn)(mpf_class const&),
                               char const* x_text,
                               mp_bitcnt_t low_precision,
                               mp_bitcnt_t high_precision,
                               unsigned long max_ulps = 2) {
    mpf_class high_x = parse_decimal_literal(x_text, high_precision);
    mpf_class low_x = parse_decimal_literal(x_text, low_precision);
    mpf_class low_value = fn(low_x);
    mpf_class high_value = fn(high_x);
    mpf_class rounded_high(0, low_precision);
    mpf_set(rounded_high.get_mpf_t(), high_value.get_mpf_t());
    assert_within_ulp(low_value, rounded_high, low_precision, max_ulps);
}

void assert_precision_doubling_binary(
    mpf_class (*fn)(mpf_class const&, mpf_class const&),
    char const* x_text,
    char const* y_text,
    mp_bitcnt_t low_precision,
    mp_bitcnt_t high_precision,
    unsigned long max_ulps = 2) {
    mpf_class high_x = parse_decimal_literal(x_text, high_precision);
    mpf_class high_y = parse_decimal_literal(y_text, high_precision);
    mpf_class low_x = parse_decimal_literal(x_text, low_precision);
    mpf_class low_y = parse_decimal_literal(y_text, low_precision);
    mpf_class low_value = fn(low_x, low_y);
    mpf_class high_value = fn(high_x, high_y);
    mpf_class rounded_high(0, low_precision);
    mpf_set(rounded_high.get_mpf_t(), high_value.get_mpf_t());
    assert_within_ulp(low_value, rounded_high, low_precision, max_ulps);
}

void test_compile_time_surface() {
    static_assert(std::same_as<decltype(pi(std::declval<mp_bitcnt_t>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(log_two(std::declval<mp_bitcnt_t>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(log(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(log1p(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(exp(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(expm1(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(sin(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(cos(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<decltype(atan(std::declval<mpf_class const&>())),
                               mpf_class>);
    static_assert(std::same_as<
                  decltype(atan2(std::declval<mpf_class const&>(),
                                 std::declval<mpf_class const&>())),
                  mpf_class>);
    static_assert(std::same_as<
                  decltype(pow(std::declval<mpf_class const&>(),
                               std::declval<mpf_class const&>())),
                  mpf_class>);
}

void test_constants() {
    mpf_class p = pi(static_cast<mp_bitcnt_t>(192));
    assert(p.get_prec() == gmpxx_detail::effective_mpf_prec(192));
    assert(p > mpf_class(3, p.get_prec()));
    assert(p < mpf_class(4, p.get_prec()));

    mpf_class l2 = log_two(static_cast<mp_bitcnt_t>(192));
    assert(l2.get_prec() == p.get_prec());
    assert(l2 > mpf_class(0, l2.get_prec()));
    assert(l2 < mpf_class(1, l2.get_prec()));
}

void test_reference_literals() {
    constexpr mp_bitcnt_t target_precision = 256;
    constexpr mp_bitcnt_t literal_precision = 768;

    struct unary_case {
        mpf_class (*fn)(mpf_class const&);
        char const* input;
        char const* reference;
        unsigned long max_ulps;
    };

    mpf_class pi_reference_hi = parse_decimal_literal(
        "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899"
        "8628034825342117067982148086513282306647093844609550582231725359408128481117450284",
        literal_precision);
    mpf_class pi_reference(0, target_precision);
    mpf_set(pi_reference.get_mpf_t(), pi_reference_hi.get_mpf_t());
    assert_within_ulp(pi(target_precision), pi_reference, target_precision, 1);

    mpf_class log_two_reference_hi = parse_decimal_literal(
        "0.69314718055994530941723212145817656807550013436025525412068000949339362196969471"
        "56058633269964186875420014810205706857336855202357581305570326707516350759619307",
        literal_precision);
    mpf_class log_two_reference(0, target_precision);
    mpf_set(log_two_reference.get_mpf_t(), log_two_reference_hi.get_mpf_t());
    assert_within_ulp(log_two(target_precision), log_two_reference,
                      target_precision, 1);

    unary_case cases[] = {
        {log1p, "0.1",
         "0.09531017980432486004395212328076509222060536530864419918523980816300101423588423"
         "283905750291303649307274794184585174988884604369351298063868901502170232637556873",
         2},
        {log, "25",
         "3.21887582486820074920151866645237527905120270853703544382529578294835797541531552"
         "926026775618635922159993260604343112579944801045864935239926723323492741145510435",
         2},
        {exp, "1",
         "2.71828182845904523536028747135266249775724709369995957496696762772407663035354759"
         "457138217852516642742746639193200305992181741359662904357290033429526059563073814",
         2},
        {expm1, "0.1",
         "0.10517091807564762481170782649024666822454719473751871879286328944096796674765462"
         "180826680334383576123364162622389881639224377083652885920639130690370248999245655",
         2},
        {sin, "1",
         "0.84147098480789650665250232163029899962256306079837106567275170999191040439123966"
         "894863974354305269585434903790792067429325911892099189888119341032772921240948079",
         4},
        {cos, "1",
         "0.54030230586813971740093660744297660373231042061792222767009725538110039477447176"
         "401902652510870988844205719922104139221278325511597186845837433912379536774980850",
         4},
        {atan, "1",
         "0.78539816339744830961566084581987572104929234984377645524373614807695410157155224"
         "965700870633552926699553662053457075766177346115238764555793134795203212028936257",
         4},
    };

    for (auto const& c : cases) {
        mpf_class input = parse_decimal_literal(c.input, target_precision);
        mpf_class reference_hi =
            parse_decimal_literal(c.reference, literal_precision);
        mpf_class reference(0, target_precision);
        mpf_set(reference.get_mpf_t(), reference_hi.get_mpf_t());
        assert_within_ulp(c.fn(input), reference, target_precision, c.max_ulps);
    }

    mpf_class atan2_reference_hi = parse_decimal_literal(
        "0.78539816339744830961566084581987572104929234984377645524373614807695410157155224"
        "965700870633552926699553662053457075766177346115238764555793134795203212028936257",
        literal_precision);
    mpf_class atan2_reference(0, target_precision);
    mpf_set(atan2_reference.get_mpf_t(), atan2_reference_hi.get_mpf_t());
    assert_within_ulp(atan2(mpf_class(1, target_precision),
                            mpf_class(1, target_precision)),
                      atan2_reference, target_precision, 4);

    mpf_class pow_reference_hi = parse_decimal_literal(
        "1.41421356237309504880168872420969807856967187537694807317667973799073247846210703"
        "885038753432764157273501384623091229702492483605585073721264412149709993583141322",
        literal_precision);
    mpf_class pow_reference(0, target_precision);
    mpf_set(pow_reference.get_mpf_t(), pow_reference_hi.get_mpf_t());
    assert_within_ulp(pow(mpf_class(2, target_precision),
                          parse_decimal_literal("0.5", target_precision)),
                      pow_reference, target_precision, 4);
}

void test_precision_doubling() {
    assert_within_ulp(pi(128), rounded_to(pi(256), static_cast<mp_bitcnt_t>(128)),
                      128, 2);
    assert_within_ulp(pi(512), rounded_to(pi(1024), static_cast<mp_bitcnt_t>(512)),
                      512, 2);
    assert_within_ulp(log_two(128),
                      rounded_to(log_two(256), static_cast<mp_bitcnt_t>(128)),
                      128, 2);
    assert_within_ulp(log_two(512),
                      rounded_to(log_two(1024), static_cast<mp_bitcnt_t>(512)),
                      512, 2);

    assert_precision_doubling(log1p, "0.1", 128, 256);
    assert_precision_doubling(log1p, "0.1", 512, 1024);
    assert_precision_doubling(log, "25", 128, 256);
    assert_precision_doubling(log, "25", 512, 1024);
    assert_precision_doubling(exp, "1", 128, 256);
    assert_precision_doubling(exp, "1", 512, 1024);
    assert_precision_doubling(expm1, "0.1", 128, 256);
    assert_precision_doubling(expm1, "0.1", 512, 1024);
    assert_precision_doubling(sin, "1", 128, 256, 4);
    assert_precision_doubling(sin, "1", 512, 1024, 4);
    assert_precision_doubling(cos, "1", 128, 256, 4);
    assert_precision_doubling(cos, "1", 512, 1024, 4);
    assert_precision_doubling(atan, "1", 128, 256, 4);
    assert_precision_doubling(atan, "1", 512, 1024, 4);
    assert_precision_doubling_binary(atan2, "1", "1", 128, 256, 4);
    assert_precision_doubling_binary(pow, "1.1", "4.4", 128, 256, 4);
    assert_precision_doubling_binary(pow, "1.1", "4.4", 512, 1024, 4);
}

void test_exact_special_values() {
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(160);
    mpf_class zero(0, prec);
    mpf_class one(1, prec);
    mpf_class two(2, prec);
    mpf_class three(3, prec);

    assert(log(one) == zero);
    assert(log1p(zero) == zero);
    assert(exp(zero) == one);
    assert(expm1(zero) == zero);
    assert(sin(zero) == zero);
    assert(cos(zero) == one);
    assert(atan(zero) == zero);
    assert(atan2(zero, one) == zero);

    assert(pow(two, three) == mpf_class(8, prec));
    assert_close(pow(two, mpf_class(-3, prec)), mpf_class("0.125", prec), 120);
}

void test_identities() {
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(192);
    mpf_class x("0.125", prec);
    mpf_class one(1, prec);
    mpf_class two(2, prec);

    assert_close(exp(log(two)), two, 120);
    assert_close(log(exp(x)), x, 110);
    assert_close(log1p(x), log(one + x), 110);
    assert_close(expm1(x), exp(x) - one, 110);

    mpf_class p = pi(prec);
    mpf_class half_pi = p;
    mpf_div_2exp(half_pi.get_mpf_t(), half_pi.get_mpf_t(), 1);
    assert_close(sin(half_pi), one, 100);
    assert_close(cos(half_pi), mpf_class(0, prec), 100);
    assert_close(atan(one), half_pi / 2, 100);
    assert_close(atan2(one, one), half_pi / 2, 100);
}

void test_near_zero_and_near_one_paths() {
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(256);
    mpf_class tiny = two_to_minus(120, prec);
    mpf_class one(1, prec);

    assert(abs(log1p(tiny) - tiny) < tiny * tiny);
    assert(abs(expm1(tiny) - tiny) < tiny * tiny);
    assert(abs(exp(tiny) - (one + tiny)) < tiny * tiny);

    mpf_class near_one = one + two_to_minus(80, prec);
    assert_within_ulp(log(near_one), log1p(two_to_minus(80, prec)), prec, 2);
}

void test_trig_reduction_and_atan2_axes() {
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(256);
    mpf_class one(1, prec);
    mpf_class zero(0, prec);
    mpf_class p = pi(prec);
    mpf_class pio2 = p;
    mpf_div_2exp(pio2.get_mpf_t(), pio2.get_mpf_t(), 1);

    mpf_class x = pio2 + two_to_minus(80, prec);
    assert(abs(cos(x) + two_to_minus(80, prec)) < two_to_minus(150, prec));
    assert_close((sin(x) * sin(x)) + (cos(x) * cos(x)), one, 120);

    assert_within_ulp(atan2(zero, one), zero, prec, 1);
    assert_within_ulp(atan2(zero, -one), p, prec, 1);
    assert_within_ulp(atan2(one, zero), pio2, prec, 1);
    assert_within_ulp(atan2(-one, zero), -pio2, prec, 1);
    assert_within_ulp(atan2(zero, zero), zero, prec, 1);
}

void test_pow_integer_and_domain_cases() {
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(192);
    assert(pow(mpf_class(5, prec), mpf_class(3, prec)) == mpf_class(125, prec));
    assert(pow(mpf_class(-2, prec), mpf_class(3, prec)) == mpf_class(-8, prec));

    bool threw = false;
    try {
        (void)pow(mpf_class(-2, prec), mpf_class("0.5", prec));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        (void)pow(mpf_class(0, prec), mpf_class(-1, prec));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);
}

void test_precision_policy() {
    mpf_class a("0.25", static_cast<mp_bitcnt_t>(128));
    mpf_class b("2.0", static_cast<mp_bitcnt_t>(320));

    assert(exp(a).get_prec() == a.get_prec());
    assert(log(b).get_prec() == b.get_prec());
    assert(sin(a).get_prec() == a.get_prec());
    assert(atan2(a, b).get_prec() == b.get_prec());
    assert(pow(a, b).get_prec() == b.get_prec());
}

void test_domain_errors() {
    mp_bitcnt_t prec = static_cast<mp_bitcnt_t>(128);
    bool threw = false;
    try {
        (void)log(mpf_class(0, prec));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        (void)log(mpf_class(-1, prec));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        (void)log1p(mpf_class(-1, prec));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        (void)pow(mpf_class(0, prec), mpf_class(0, prec));
    } catch (std::domain_error const&) {
        threw = true;
    }
    assert(threw);
}

}  // namespace

int main() {
    test_compile_time_surface();
    test_constants();
    test_reference_literals();
    test_precision_doubling();
    test_exact_special_values();
    test_identities();
    test_near_zero_and_near_one_paths();
    test_trig_reduction_and_atan2_axes();
    test_pow_integer_and_domain_cases();
    test_precision_policy();
    test_domain_errors();
    return 0;
}
