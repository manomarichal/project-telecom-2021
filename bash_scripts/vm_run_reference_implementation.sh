set -e
echo "================= UPDATING START SCRIPTS ===================="
rm -f /home/student/click/scripts/start_click.sh
cp /home/student/Desktop/project-telecom-2021/bash_scripts/start_scripts/reference_scripts/start_script_reference.sh /home/student/click/scripts/start_click.sh
rm -f /home/student/click-reference/solution/start_click.sh
cp /home/student/Desktop/project-telecom-2021/bash_scripts/start_scripts/student_scripts/start_script_student.sh /home/student/click-reference/solution/start_click.sh
echo "================= RUNNING CLICK ===================="
chmod +x /home/student/click-reference/solution/start_click.sh
/home/student/click-reference/solution/start_click.sh
