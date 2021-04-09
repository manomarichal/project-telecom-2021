echo "emptying igmp folder"
rm -rf /home/student/click/elements/local/igmp
mkdir /home/student/click/elements/local/igmp
echo "copying new files"
cp /home/student/Desktop/project-telecom-2021/elements/local/igmp/IGMPClientSide.cc /home/student/click/elements/local/igmp/IGMPClientSide.cc
echo "updating reference start.sh"
rm /home/student/click/scripts/start_click.sh
cp /home/student/Desktop/project-telecom/igmp_opgave/scripts/start_scripts/reference_scripts/start_script_client_reference.sh /home/student/click/scripts/start_click.sh
echo "updating student start.sh"
rm /home/student/click-reference/solution/start_click.sh
cp /home/student/Desktop/project-telecom/igmp_opgave/scripts/start_scripts/student_scripts/start_script_client_student.sh /home/student/click-reference/solution/start_click.sh
echo "building click"
cd /home/student/click
make elemlist
make

