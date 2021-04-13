set -e
echo "================= RESTORING REFERENCE FILES ===================="
sudo rm -rf /home/student/click/scripts/library
mkdir /home/student/click/scripts/library
cp -r /home/student/Desktop/project-telecom-2021/igmp_opgave/scripts/library /home/student/click/scripts
echo "================= UPDATING START SCRIPTS ===================="
rm -f /home/student/click-reference/solution/start_click.sh
cp /home/student/Desktop/project-telecom-2021/bash_scripts/start_scripts/reference_scripts/reference.sh /home/student/click-reference/solution/start_click.sh
echo "================= RUNNING CLICK ===================="
chmod +x /home/student/click-reference/solution/start_click.sh
/home/student/click-reference/solution/start_click.sh
