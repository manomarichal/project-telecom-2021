set -e
echo "================= REMOVING OLD FILES ===================="
sudo rm -rf /home/student/click/elements/local/igmp
rm -rf /home/student/click/scripts/library
mkdir /home/student/click/elements/local/igmp
mkdir /home/student/click/scripts/library
echo "================= COPYING NEW FILES ===================="
cp -r /home/student/Desktop/project-telecom-2021/elements/local/igmp /home/student/click/elements/local/igmp
cp -r /home/student/Desktop/project-telecom-2021/library /home/student/click/scripts
echo "================= UPDATING START SCRIPTS ===================="
rm -f /home/student/click/scripts/start_click.sh
cp /home/student/Desktop/project-telecom-2021/bash_scripts/start_scripts/student_scripts/client+router_student.sh /home/student/click/scripts/start_click.sh
rm -f /home/student/click-reference/solution/start_click.sh
cp /home/student/Desktop/project-telecom-2021/bash_scripts/start_scripts/reference_scripts/client+router_reference.sh /home/student/click-reference/solution/start_click.sh
echo "================= BUILDING CLICK ===================="
cd /home/student/click
make elemlist
make
echo "================= RUNNING CLICK ===================="
chmod +x /home/student/click-reference/solution/start_click.sh
chmod +x /home/student/click/scripts/start_click.sh
/home/student/click-reference/solution/start_click.sh
/home/student/click/scripts/start_click.sh
