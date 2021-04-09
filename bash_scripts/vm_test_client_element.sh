set -e
echo "emptying igmp folder"
rm -rf /home/student/click/elements/local/igmp
mkdir /home/student/click/elements/local/igmp
echo "copying new files"
cp -r /home/student/Desktop/project-telecom-2021/elements/local/igmp /home/student/click/elements/local/igmp
echo "updating reference start.sh"
rm -f /home/student/click/scripts/start_click.sh
cp /home/student/Desktop/project-telecom-2021/igmp_opgave/scripts/start_scripts/reference_scripts/start_script_client_reference.sh /home/student/click/scripts/start_click.sh
echo "updating student start.sh"
rm -f /home/student/click-reference/solution/start_click.sh
cp /home/student/Desktop/project-telecom-2021/igmp_opgave/scripts/start_scripts/student_scripts/start_script_client_student.sh /home/student/click-reference/solution/start_click.sh
echo "building click"
cd /home/student/click
make elemlist
make
echo "running click"
chmod +x /home/student/click-reference/solution/start_click.sh
chmod +x /home/student/click/scripts/start_click.sh
sudo /home/student/click-reference/solution/start_click.sh
sudo /home/student/click/scripts/start_click.sh
