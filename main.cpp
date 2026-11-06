/****************/ 
// Mã sinh viên: 202418929
// Họ tên: Hoàng Chí Kiệt
/****************/ 

#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace cachep {
    void printLine() {
        cout << "------------------------------------\n";
    }
}

class Employee {
protected:
    string empId;
    string name;
    string dept;
    double bonus;

public:
    Employee(string id, string n) : empId(id), name(n), dept("Unassigned"), bonus(0) {}
    
    Employee(string id, string n, string d) : empId(id), name(n), dept(d), bonus(0) {}

    // Overload addBonus
    void addBonus(double amount) {
        if (amount > 0) bonus += amount;
    }

    void addBonus(double amount, string reason) {
        if (amount > 0 && reason != "") bonus += amount;
    }

    void addBonus(double rate, double refAmount, string reason) {
        if (rate > 0 && rate <= 0.5 && refAmount > 0 && reason != "") {
            bonus += (rate * refAmount);
        }
    }

    string getId() { return empId; }
    string getDept() { return dept; }

    virtual double calcGrossPay() = 0; // pure virtual
    virtual string getType() = 0;
    
    virtual void printInfo() {
        cout << "ID: " << empId << " | Ten: " << name 
             << " | Phong: " << dept << " | Thuong: " << (long long)bonus << " VND";
    }

    virtual ~Employee() {}
};

class SalariedEmployee : public Employee {
private:
    double monthlySalary;
    double allowance; // phu cap trach nhiem

public:
    SalariedEmployee(string id, string n, double sal) 
        : Employee(id, n), monthlySalary(sal < 0 ? 0 : sal), allowance(0) {}

    SalariedEmployee(string id, string n, string d, double sal, double allow) 
        : Employee(id, n, d) {
        monthlySalary = (sal >= 0) ? sal : 0;
        allowance = (allow >= 0) ? allow : 0;
    }

    double calcGrossPay() override {
        return monthlySalary + allowance + bonus;
    }

    string getType() override { return "Salaried"; }

    void printInfo() override {
        Employee::printInfo();
        cout << " | Thuc lanh: " << (long long)calcGrossPay() << " VND\n";
    }
};

class HourlyEmployee : public Employee {
private:
    double rate;
    double hours;

public:
    HourlyEmployee(string id, string n, double r) 
        : Employee(id, n), rate(r < 0 ? 0 : r), hours(0) {}

    HourlyEmployee(string id, string n, string d, double r, double h) 
        : Employee(id, n, d) {
        rate = (r >= 0) ? r : 0;
        hours = (h >= 0 && h <= 250) ? h : 0; // limit 250h
    }

    double calcGrossPay() override {
        double base = 0;
        if (hours <= 160) {
            base = hours * rate;
        } else {
            // tinh OT x1.5 cho gio vuot 160
            base = (160 * rate) + ((hours - 160) * rate * 1.5);
        }
        return base + bonus;
    }

    string getType() override { return "Hourly"; }

    void printInfo() override {
        Employee::printInfo();
        cout << " | Thuc lanh: " << (long long)calcGrossPay() << " VND\n";
    }
};

class SalesEmployee : public Employee {
private:
    double baseSal;
    double revenue;
    double commRate; // ti le hoa hong

public:
    SalesEmployee(string id, string n, double base) 
        : Employee(id, n), baseSal(base < 0 ? 0 : base), revenue(0), commRate(0) {}

    SalesEmployee(string id, string n, string d, double base, double rev, double cRate) 
        : Employee(id, n, d) {
        baseSal = (base >= 0) ? base : 0;
        revenue = (rev >= 0) ? rev : 0;
        commRate = (cRate >= 0 && cRate <= 0.3) ? cRate : 0;
    }

    void updateRevenue(double rev) {
        if (rev >= 0) revenue = rev;
    }

    double calcGrossPay() override {
        return baseSal + (revenue * commRate) + bonus;
    }

    string getType() override { return "Sales"; }

    void printInfo() override {
        Employee::printInfo();
        cout << " | Thuc lanh: " << (long long)calcGrossPay() << " VND\n";
    }
};

class Payroll {
private:
    string period;
    vector<Employee*> emps;

public:
    Payroll(string p) : period(p) {}

    bool addEmp(Employee* e) {
        if (!e) return false;
        // check trung id
        for (int i = 0; i < emps.size(); i++) {
            if (emps[i]->getId() == e->getId()) {
                cout << "Loi: ID " << e->getId() << " bi trung!\n";
                return false;
            }
        }
        emps.push_back(e);
        return true;
    }

    double calcTotal() {
        double sum = 0;
        for (auto e : emps) sum += e->calcGrossPay();
        return sum;
    }

    double calcByDept(string dept) {
        double sum = 0;
        for (auto e : emps) {
            if (e->getDept() == dept) sum += e->calcGrossPay();
        }
        return sum;
    }

    Employee* getHighestPaid() {
        if (emps.empty()) return nullptr;
        Employee* best = emps[0];
        for (auto e : emps) {
            if (e->calcGrossPay() > best->calcGrossPay()) best = e;
        }
        return best;
    }

    void display() {
        cout << "\n=== BANG LUONG THANG: " << period << " ===\n";
        for (auto e : emps) e->printInfo();
        cachep::printLine();
        cout << "Tong luong ca cong ty: " << (long long)calcTotal() << " VND\n";
    }
};

int main() {
    Payroll pr("2026-09");

    // Test case 1: Salaried - Dữ liệu chuẩn
    SalariedEmployee e1("E001", "Em Toi Do", "Dao tao", 15000000, 2000000);
    e1.addBonus(1000000); // Dùng addBonus(amount)
    pr.addEmp(&e1);

    // Test case 2: Hourly - Dưới 160h (Không OT)
    HourlyEmployee e2("E002", "Hoang Chi Kiet (Ban Chinh)", "Ho tro", 100000, 150);
    e2.addBonus(500000, "Thuong fix bug xuyen dem"); // Dùng addBonus(amount, reason)
    pr.addEmp(&e2);

    // Test case 3: Hourly - Vượt 160h (Có OT x1.5)
    HourlyEmployee e3("E003", "Kiet Clone 1", "Ho tro", 100000, 170);
    pr.addEmp(&e3);

    // Test case 4: Sales - Dữ liệu chuẩn
    SalesEmployee e4("E004", "Kiet Clone 2", "Kinh doanh", 8000000, 200000000, 0.05);
    e4.addBonus(0.02, 50000000, "Chot don khung"); // Dùng addBonus(rate, ref, reason)
    pr.addEmp(&e4);

    // Test case 5: Salaried - Nhập lương âm (Code tự ép về 0)
    SalariedEmployee e5("E005", "Thuc Tap Sinh", "IT", -5000000, 1000000);
    pr.addEmp(&e5);

    // Test case 6: Hourly - Cày cuốc quá 250h/tháng (Giờ tự ép về 0 theo logic constructor)
    HourlyEmployee e6("E006", "Thanh Cay Cuoc", "Ho tro", 100000, 300);
    pr.addEmp(&e6);

    // Test case 7: Sales - Tỷ lệ hoa hồng > 0.3 (Tự ép về 0)
    SalesEmployee e7("E007", "Chua Te Lua Ga", "Kinh doanh", 5000000, 100000000, 0.5);
    pr.addEmp(&e7);

    cout << "\n--- LOG KIEM TRA BAO LOI ---\n";

    // Test case 8: Cố tình nhập trùng mã nhân viên đã có (E001)
    SalariedEmployee e8("E001", "Ke Mao Danh", "Dao tao", 10000000, 0);
    pr.addEmp(&e8); // Sẽ in ra cảnh báo lỗi trùng ID và không thêm vào danh sách

    // Test case 9: Cố tình bào tiền công ty (rate > 0.5 -> Hàm addBonus tự từ chối)
    SalesEmployee e9("E009", "Nhan Vien Tham Lam", "Marketing", 6000000, 50000000, 0.1);
    e9.addBonus(0.8, 10000000, "Doi thuong qua dang"); // rate 0.8 vô hiệu, bonus vẫn = 0
    pr.addEmp(&e9);

    // Test case 10: Truyền con trỏ NULL vào danh sách
    pr.addEmp(nullptr); // Bị bỏ qua âm thầm, code không crash

    // Hiển thị kết quả cuối cùng
    pr.display();

    cout << "\nTong luong rieng phong 'Ho tro': " << (long long)pr.calcByDept("Ho tro") << " VND\n";
    cout << "Tong luong rieng phong 'Kinh doanh': " << (long long)pr.calcByDept("Kinh doanh") << " VND\n";

    Employee* vip = pr.getHighestPaid();
    if (vip) {
        cout << "Nhan vien luong cao nhat cong ty: " << vip->getId() 
             << " - " << (long long)vip->calcGrossPay() << " VND\n";
    }

    return 0;
}