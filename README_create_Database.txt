To create the Database and give the DB-User the right privileges use the following statements:

sudo mysql -u root

# The mariadb prompt will be shown: 
# MariaDB [(none)]>
# Now use this SQL Commands:

create database rf24hub;
use rf24hub;

create user '<user>'@'<host>' identified by '<password>';

GRANT USAGE ON *.* TO '<user>'@'<host>' IDENTIFIED BY '<password>';
# or
GRANT USAGE ON *.* TO '<user>'@'%' IDENTIFIED BY '<password>';

GRANT ALL PRIVILEGES ON `rf24hub`.* TO '<user>'@'<host>';



