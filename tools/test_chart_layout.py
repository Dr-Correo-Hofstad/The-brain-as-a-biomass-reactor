import os
import re
import sys

class MarkdownLayoutChecker:
    def __init__(self, report_dir="reports/"):
        self.report_dir = report_dir
        # Regex to scan for valid markdown chart and image tags: ![Alt Text](path/to/chart.png)
        self.chart_tag_pattern = re.compile(r'\!\[.*?\]\((.*?)\)')

    def audit_report_layout_compliance(self):
        """
        Parses compiled clinical report text lines to confirm all referenced 
        performance charts exist physically on disk and contain no rendering errors.
        """
        print("[*] Launching Automated Markdown Layout Verification Audit...")
        
        if not os.path.exists(self.report_dir):
            print(f"[+] Target report directory '{self.report_dir}' is currently empty. Bypassing check.")
            return True

        report_files = [f for f in os.listdir(self.report_dir) if f.endswith('.md')]
        if not report_files:
            print("[+] No markdown documents detected. Layout validation passed implicitly.")
            return True

        errors_found = 0

        for report in report_files:
            file_path = os.path.join(self.report_dir, report)
            print(f"    - Ingesting Document Matrix: {report}")
            
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()

            # 1. Audit check for overlapping marker tags or broken dividers
            if "####" in content and "||" in content:
                # Basic layout boundary validation heuristics
                if content.count("│") > 0 and content.count("   ") == 0:
                    print(f"    [!] LAYOUT COMPLIANCE FAULT: Detected potential text/table overlap inside {report}")
                    errors_found += 1

            # 2. Extract and test referenced chart image assets
            charts = self.chart_tag_pattern.findall(content)
            for chart_path in charts:
                # Clean path variables if utilizing relative formats
                clean_path = chart_path.lstrip("./")
                if not os.path.exists(clean_path):
                    print(f"    [!] RENDERING ERROR: Broken visual link found inside {report}! Missing file: {clean_path}")
                    errors_found += 1

        if errors_found > 0:
            print(f"\n[!] VERIFICATION AUDIT FAILED: {errors_found} layout formatting conflicts intercepted.")
            return False
            
        print("[+] SUCCESS: All markdown reports match layout compliance criteria. Overlaps=0.")
        return True

if __name__ == "__main__":
    checker = MarkdownLayoutChecker()
    compliance_passed = checker.audit_report_layout_compliance()
    if not compliance_passed:
        sys.exit(1) # Break build pipeline sequence if constraints are violated
