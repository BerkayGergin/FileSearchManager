
from pathlib import Path
from random import Random

ROOT = Path("samples/demo_corpus")

TOPICS = {
    "operating_systems": [
        ("virtual_memory", "Virtual memory maps logical addresses to physical memory."),
        ("cpu_scheduling", "CPU scheduling distributes processor time among processes."),
        ("deadlocks", "A deadlock can occur when processes wait for held resources."),
        ("page_replacement", "Page replacement determines which memory page is removed."),
        ("ipc", "Interprocess communication allows processes to exchange information."),
        ("filesystem_cache", "File system caching reduces repeated disk operations."),
        ("race_conditions", "A race condition depends on the timing of concurrent tasks."),
        ("process_creation", "Fork creates a new process with its own execution context."),
    ],
    "programming": [
        ("exception_handling", "Exceptions provide structured error handling."),
        ("unit_testing", "Unit tests verify individual functions and expected outputs."),
        ("data_structures", "Data structures affect search and insertion complexity."),
        ("api_design", "An API defines how software components communicate."),
        ("python_scripting", "Python automates file processing and repetitive tasks."),
        ("cpp_templates", "C++ templates support reusable generic algorithms."),
        ("debugging_session", "Debugging uses logs and breakpoints to identify defects."),
        ("thread_pool", "A thread pool reuses workers to execute queued tasks."),
    ],
    "databases": [
        ("indexes", "Database indexes accelerate selected query operations."),
        ("transactions", "Transactions protect consistency across multiple updates."),
        ("query_planning", "The database optimizer selects an execution plan."),
        ("schema_migrations", "Schema migrations track structural database changes."),
        ("backup_recovery", "Backups allow recovery after data loss."),
        ("replication", "Database replication maintains copies across servers."),
        ("database_cache", "Caching reduces repeated database reads."),
        ("analytics", "Analytical queries aggregate large amounts of information."),
    ],
    "networking": [
        ("tcp_connection", "TCP provides ordered and reliable data delivery."),
        ("dns_lookup", "DNS resolves domain names into network addresses."),
        ("http_requests", "HTTP requests transfer resources between clients and servers."),
        ("network_latency", "Latency measures delay in network communication."),
        ("routing", "Routers select paths for packets across networks."),
        ("load_balancing", "Load balancing distributes traffic among servers."),
        ("websocket", "WebSocket supports persistent bidirectional communication."),
        ("firewall_rules", "Firewalls filter network traffic using security rules."),
    ],
    "cybersecurity": [
        ("access_control", "Access control restricts resources to authorized users."),
        ("audit_logs", "Audit logs record security-relevant system events."),
        ("encryption", "Encryption protects sensitive information."),
        ("session_management", "Session management controls authenticated sessions."),
        ("threat_modeling", "Threat modeling identifies possible attack scenarios."),
        ("input_validation", "Input validation rejects unexpected or unsafe data."),
        ("secret_storage", "Secrets should be stored outside application source code."),
        ("incident_response", "Incident response documents detection and recovery."),
    ],
    "project_documents": [
        ("sprint_planning", "The team prioritizes tasks for the next development sprint."),
        ("support_ticket", "A support ticket records a reported software problem."),
        ("meeting_notes", "Meeting notes summarize decisions and assigned actions."),
        ("release_plan", "The release plan defines milestones and deployment checks."),
        ("onboarding", "Onboarding documentation explains the development environment."),
        ("cost_review", "The cost review evaluates infrastructure and maintenance."),
        ("incident_summary", "An incident summary records impact and corrective actions."),
        ("requirements", "Requirements define expected features and constraints."),
    ],
}

SUPPORTING_NOTES = {
    "operating_systems": [
        "Memory management protects address spaces between processes.",
        "Threads in the same process can share memory.",
        "A mutex prevents simultaneous modification of shared state.",
        "Context switching saves and restores execution information.",
        "The kernel controls hardware resources and process execution.",
        "Page faults may require loading data from secondary storage.",
        "Synchronization is important for predictable concurrent results.",
    ],
    "programming": [
        "Automated tests make regression errors easier to detect.",
        "Clear function interfaces improve maintainability.",
        "Error messages should explain why an operation failed.",
        "Code review helps detect implementation mistakes.",
        "Performance profiling identifies expensive operations.",
        "Memory safety requires careful resource management.",
        "Logging makes difficult bugs easier to reproduce.",
    ],
    "databases": [
        "SQL queries retrieve and modify stored records.",
        "An index can speed up lookups but increases write overhead.",
        "Transactions support consistent database operations.",
        "Query execution time depends on data size and indexing.",
        "A cache may keep frequently accessed records in memory.",
        "Backup verification is part of recovery planning.",
        "Normalization reduces some forms of data duplication.",
    ],
    "networking": [
        "Packet loss can reduce application performance.",
        "Network monitoring helps identify connection failures.",
        "DNS caching reduces repeated name resolution requests.",
        "TCP connections maintain transport-layer state.",
        "Request timeouts prevent indefinite network waits.",
        "Load balancing can improve service availability.",
        "TLS protects data exchanged over network connections.",
    ],
    "cybersecurity": [
        "Authentication confirms a user's claimed identity.",
        "Authorization determines which operations are permitted.",
        "Security logs support incident investigations.",
        "Input validation reduces exposure to malformed requests.",
        "Encryption protects data during transmission and storage.",
        "Least privilege limits unnecessary resource access.",
        "Incident handling requires clear error reporting.",
    ],
    "project_documents": [
        "The team records completed work in Git commits.",
        "Each developer reviews their assigned module.",
        "Integration testing verifies communication between components.",
        "Performance metrics are included in the progress report.",
        "The meeting includes a demonstration of working features.",
        "Open issues are assigned to responsible team members.",
        "Release readiness depends on passing required tests.",
    ],
}

if ROOT.exists():
    raise SystemExit(
        f"{ROOT} already exists. Existing files were not overwritten."
    )

for category, topics in TOPICS.items():
    directory = ROOT / category
    directory.mkdir(parents=True, exist_ok=True)

    for index, (filename, description) in enumerate(topics):
        notes = SUPPORTING_NOTES[category]

        rng = Random(361 + index + sum(map(ord, category)))
        selected_notes = rng.sample(notes, k=5)

        title = filename.replace("_", " ").title()

        lines = [
            f"Document title: {title}",
            f"Category: {category}",
            f"Document reference: {category}-{index + 1:03d}",
            "",
            "Summary:",
            description,
            "",
            "Technical observations:",
            *selected_notes,
            "",
            "Review status: Completed",
            "Source: FileSearchManager demonstration dataset",
        ]

        path = directory / f"{filename}.txt"
        path.write_text("\n".join(lines) + "\n", encoding="utf-8")

edge = ROOT / "edge_cases"
edge.mkdir(parents=True, exist_ok=True)

special_files = {
    "empty.txt": "",
    "no_match.txt":
        "The garden contains an oak tree and a small pond.\n",
    "uppercase_memory.txt":
        "MEMORY MANAGEMENT\nPROCESS SCHEDULING\n",
    "turkish_notes.txt":
        "İşletim sistemleri bellek yönetimini sağlar.\n"
        "İş parçacıkları eşzamanlı çalışabilir.\n",
    "no_final_newline.txt":
        "The operating system handles memory allocation",
}

for filename, content in special_files.items():
    (edge / filename).write_text(content, encoding="utf-8")

all_files = sorted(ROOT.rglob("*.txt"))

print("Demo corpus created!")
print(f"Total files: {len(all_files)}")


for keyword in ["memory", "thread", "database", "error", "network"]:
    matching_lines = 0

    for path in all_files:
        text = path.read_text(encoding="utf-8")
        matching_lines += sum(
            keyword in line for line in text.splitlines()
        )

    print(f"{keyword}: {matching_lines} matching lines")
