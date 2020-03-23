class Measure < ApplicationRecord

    def self.temp_by_time

        find_by_sql(<<-SQL
            SELECT
            #date_trunc('minute', created_at) AS date_time,
            date_trunc('minute', created_at),    
            temp_out,
            temp_in
            FROM measures
            GROUP BY created_at, temp_out, temp_in
            ORDER BY created_at
            SQL
          ).map do |row|
            [
                row['date_time'],
                row.temp_out,
                row.temp_in,
            ]
          end
    end

end
