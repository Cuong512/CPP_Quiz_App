cmake_minimum_required(VERSION 3.10)

# Tên dự án của bạn
project(TracNghiemPro)

# Chuẩn C++ (Bạn đang dùng C++17 hoặc mới hơn)
set(CMAKE_CXX_STANDARD 17)

# Khai báo file chạy (Executable) và liệt kê TẤT CẢ các file .cpp, .h vào đây
add_executable(TracNghiemPro
    main.cpp
    Quiz.cpp
    Quiz.h
    Question.h
    Utils.h
)

# (Tuỳ chọn) Copy file questions.txt vào thư mục build mỗi khi chạy
# Để đảm bảo chương trình luôn tìm thấy file dữ liệu
configure_file(questions.txt ${CMAKE_CURRENT_BINARY_DIR}/questions.txt COPYONLY)