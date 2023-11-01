/*******************************************************************************
 * Data   : 2023/10/20
 * Author : yongheng
*******************************************************************************/

module;
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
export module BigNumber;

template<typename T>
concept StringType =
std::is_same_v<T,std::vector<int>> ||
std::is_same_v<T,std::string> ||
std::is_same_v<T,char *> ||
std::is_same_v<T,const char *>;


export namespace BigNumber {

class BigInt {
    bool sign = true;
    std::string number = "";

    // 大于
    bool AbsGt(const BigInt& big_int) const {
        if (number.size() > big_int.number.size()) {
            return true;
        }
        else if (number.size() == big_int.number.size()) {
            for (int i = number.size()-1; i >= 0; --i) {
                if (number[i] != big_int.number[i])
                    return number[i] > big_int.number[i];
            }
            return false;
        }
        else {
            return false;
        }
    }

    bool AbsGe(const BigInt& big_int) const {
        if (number.size() > big_int.number.size()) {
            return true;
        }
        else if (number.size() == big_int.number.size()) {
            for (int i = number.size()-1; i >= 0; --i) {
                if (number[i] != big_int.number[i])
                    return number[i] > big_int.number[i];
            }
            return true;
        }
        else {
            return false;
        }
    }


    BigInt Add(const BigInt& big_int,bool sign = true) const {
        std::string new_number(std::max(number.size(),big_int.number.size()),0);
        for (int i = 0;i < number.size(); ++i) {
            new_number[i] += number[i];
        }
        for (int i = 0;i < big_int.number.size(); ++i) {
            new_number[i] += big_int.number[i];
        }
        int j = 0;
        for (auto &ch : new_number) {
            ch += j;
            if (ch >= 10) {
                ch -= 10;
                j = 1;
            }
            else {
                j = 0;
            }
        }
        if (j == 1) {
            new_number.resize(new_number.size()+1);
            new_number[new_number.size()-1] = 1;
        }

        return BigInt(new_number,sign,false,false);
    }

    // 只能大减小
    BigInt Sub(const BigInt& big_int,bool sign = true) const {
        std::string new_number = number;
        for (int i = 0;i < big_int.number.size(); ++i) {
            new_number[i] -= big_int.number[i];
        }
        // 由于只能进行大的-小的运算，因此一定不会访问最高位
        for (int i = 0;i < number.size()-1; ++i) {
            if (!(new_number[i] >= 0 && new_number[i] <= 9)) {
                new_number[i] += 10;
                new_number[i+1] -= 1;
            }
        }
        if (new_number[new_number.size()-1] == 0) {
            new_number.resize(new_number.size()-1);
        }

        return BigInt(new_number,sign, false, false);
    }

    BigInt Mul(const BigInt& big_int) const {
        std::string new_number(number.size() + big_int.number.size(),'\0');

        int carry = 0;
        for (int i = 0;i < number.size(); ++i) {
            for (int j = 0;j < big_int.number.size() ; ++j) {
                new_number[i+j] += number[i] * big_int.number[j] + carry;
                carry = new_number[i+j] / 10;
                new_number[i+j] %= 10;
            }
            if (carry != 0) {
                new_number[i+big_int.number.size()] += number[i] * big_int.number[big_int.number.size()] + carry;
                carry = new_number[i+big_int.number.size()] / 10;
                new_number[i+big_int.number.size()] %= 10;
            }
        }


        return BigInt(new_number, true, false, false);
    }

    std::tuple<BigInt,BigInt> Div(const BigInt& big_int) const {
        if (big_int == BigInt(0)) {
            std::cerr << "被除数不为0" << std::endl;
            return {BigInt("0"),BigInt("0")};
        }


        std::string quotient = std::string(number.size(),'\0');
        std::string remainder = number;

        for (int i = number.size()-big_int.number.size();i >= 0; --i) {

            while (BigInt(std::string(remainder.begin()+i,remainder.end()),true,false, false) >= big_int) {
                auto p = BigInt(std::string(remainder.begin()+i,remainder.end()),true,false, false);
                for (auto &ch : p.number) {
                    ch += '0';
                }
                for (int j = 0;j < big_int.number.size() ; ++j) {
                    int p1 = remainder[i+j];
                    remainder[i+j] -= big_int.number[j];
                    int p2 = remainder[i+j];
                    if (remainder[i+j] < 0) {
                        remainder[i+j+1] --;
                        remainder[i+j] += 10;
                    }
                }
                quotient[i] ++;
            }
        }
        return {BigInt(quotient,true,false, false),BigInt(remainder,true,false, false)};
    }

public:
    explicit BigInt(StringType auto str_num,bool sign = true,bool need_sub_0 = true,bool need_reverse = true) {


        if (str_num[0] == '-') {
            sign = false;
            number = std::string(&str_num[1]);
        }
        else {
            number = std::string(str_num);
        }
        if (need_sub_0 == true) {
            for (auto &ch : number) {
                ch -= '0';
            }
        }

        if (need_reverse == true) {
            auto pos = std::find_if(number.begin(),number.end(),[](char ch) { return ch != '\0';});
            number = std::string(pos,number.end());
        }
        else {
            std::reverse(number.begin(),number.end());
            auto pos = std::find_if(number.begin(),number.end(),[](char ch) { return ch != '\0';});
            number = std::string(pos,number.end());
        }
        std::reverse(number.begin(),number.end());

        if (number == "") {
            number = "0";
            number[0] -= '0';
            return;
        }
        this->sign = sign;
    }

    explicit BigInt(std::integral auto num) : BigInt(std::to_string(num)){  }

    BigInt(const BigInt& big_int) {
        number = big_int.number;
    }
    BigInt& operator = (const BigInt& big_int) = default;

    friend std::ostream& operator << (std::ostream &out,const BigInt& big_int) {
        if (big_int.sign == false)
            std::cout << '-';

        for (int i = big_int.number.size()-1;i >= 0; --i) {
            std::cout << (int)big_int.number[i];
        }
        return out;
    }

    BigInt operator + (const BigInt &big_int) const {
        if (sign == true && big_int.sign == true) {
            return Add(big_int);
        }
        else if (sign == true && big_int.sign == false ||
                 sign == false && big_int.sign == true) {
            // 正大
            if (AbsGe(big_int)) {
                return Sub(big_int,sign);
            }
            else {
                return big_int.Sub(*this, big_int.sign);
            }
        }
        else {
            return Add(big_int, false);
        }
    }

    BigInt operator - (const BigInt& big_int) {

        if (sign == true && big_int.sign == true) {
            // 大 - 小
            if (AbsGe(big_int)) {
                return Sub(big_int);
            }
            else {
                return big_int.Sub(*this, false);
            }
        }
        else {
            return Add(big_int, sign);
        }
    }

    BigInt operator * (const BigInt& big_int) {
        BigInt ans = Mul(big_int);
        ans.sign = sign | big_int.sign;
        return std::move(ans);
    }
    BigInt operator / (const BigInt& big_int) {
        auto [ans,_] = Div(big_int);
        ans.sign = sign | big_int.sign;
        return std::move(ans);
    }
    BigInt operator % (const BigInt &big_int) {
        auto [_,ans] = Div(big_int);
        return ans;
    }

    bool operator > (const BigInt& big_int) {
        if (sign == true && big_int.sign == true) {
            return AbsGe(big_int);
        }
        else if (sign == true && big_int.sign == false) {
            return true;
        }
        else if (sign == false && big_int.sign == true) {
            return false;
        }
            // false false
        else {
            // -1 > -2 true   !(1 >=  2)
            // -1 > -1 false  !(1 >=  1)
            // -2 > -1 false  !(2 >=  1)
            return !AbsGe(big_int);
        }
    }
    bool operator >= (const BigInt& big_int) {
        if (sign == true && big_int.sign == true) {
            return AbsGe(big_int);
        }
        else if (sign == true && big_int.sign == false) {
            return true;
        }
        else if (sign == false && big_int.sign == true) {
            return false;
        }
            // false false
        else {
            // -1 >= -2 true   !(1 >  2)
            // -1 >= -1 true   !(1 >  1)
            // -2 >= -1 false  !(2 >  1)
            return !AbsGt(big_int);
        }
    }
    bool operator == (const BigInt& big_int) {
        if (sign != big_int.sign)
            return false;

        if (number.size() != big_int.number.size())
            return false;

        return number == big_int.number;
    }
};


namespace big_number_literals {
    BigNumber::BigInt operator ""_big_number (const char *str,std::size_t len) {
        return BigNumber::BigInt(str);
    }
}// namespace BigNumber::big_number_literals

}// namespace BigNumber
