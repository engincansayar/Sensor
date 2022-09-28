####- SQLite'ı bu linkteki talimatlara uyarak kurdum.
  https://www.tutorialspoint.com/sqlite/sqlite_installation.htm 
####- SQLite 'header'ını #include "sqlite3.h" şeklinde tanımladım.
####- Program 4 farklı isteğe(GET,POST,PUT ve DELETE) cevap veriyor.  
##- POST işlemi için curl yapılırken aşağıdaki örnekteki gibi yapılmalıdır.
  POST http://localhost:8001/?current=3.98&voltage=5468.987
##- PUT işlemi için curl yapılırken aşağıdaki örnekteki gibi yapılmalıdır.
  PUT http://localhost:8001/sensor_data?id=3&current=10.49&voltage=506.78
##- GET işlemi için curl yapılırken aşağıdaki örnekteki gibi yapılmalıdır.
  GET http://localhost:8001/sensor_data
##- DELETE işlemi için curl yapılırken aşağıdaki örnekteki gibi yapılmalıdır.
  DELETE http://localhost:8001/sensor_data
